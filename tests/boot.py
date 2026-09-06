#!/usr/bin/env python3
"""Boot real disk ELFs and exercise process and storage failure paths."""
import os
from pathlib import Path
import select
import struct
import subprocess as sp
import tempfile
import time


class Qemu:
    def __init__(self, disk=None):
        args = ['qemu-system-arm', '-M', 'versatilepb', '-cpu', 'arm926',
                '-m', '256M', '-nographic', '-audio', 'driver=none',
                '-kernel', 'arunos.bin']
        if disk:
            args += ['-drive', f'if=none,id=d,format=raw,file={disk},readonly=on',
                     '-device', 'virtio-blk-pci,drive=d,disable-modern=on']
        self.p = sp.Popen(args, stdin=sp.PIPE, stdout=sp.PIPE,
                          stderr=sp.STDOUT)
        self.pending = b''

    def expect(self, text, timeout=15):
        target = text.encode()
        deadline = time.monotonic() + timeout
        while target not in self.pending:
            remaining = deadline - time.monotonic()
            if remaining <= 0:
                raise AssertionError((text, self.pending[-1500:]))
            ready, _, _ = select.select([self.p.stdout], [], [], remaining)
            if ready:
                chunk = os.read(self.p.stdout.fileno(), 65536)
                if not chunk:
                    raise AssertionError(self.pending)
                self.pending += chunk
        before, self.pending = self.pending.split(target, 1)
        return before

    def send(self, text):
        self.p.stdin.write(text.encode() + b'\n')
        self.p.stdin.flush()

    def close(self):
        self.p.terminate()
        self.p.communicate(timeout=5)


with tempfile.TemporaryDirectory() as tmp:
    tmp = Path(tmp)
    disk = tmp / 'disk.img'
    disk.write_bytes(Path('disk.img').read_bytes())
    obj, elf = tmp / 'test.o', tmp / 'test.elf'
    sp.run(['arm-none-eabi-gcc', '-mcpu=arm926ej-s', '-marm', '-fPIC',
            '-mapcs-frame', '-nostdinc', '-nostdlib', '-fno-builtin',
            '-Iinclude', '-c', 'tests/elf_test.c', '-o', str(obj)], check=True)
    sp.run(['arm-none-eabi-ld', '-Ttext=100', str(obj), 'lib/libarunos.a',
            '-o', str(elf)], check=True)
    sp.run(['mcopy', '-i', str(disk), str(elf), '::check.elf'], check=True)
    bad = tmp / 'bad.elf'
    bad.write_bytes(b'not an elf')
    sp.run(['mcopy', '-i', str(disk), str(bad), '::bad.elf'], check=True)
    q = Qemu(disk)
    try:
        q.expect('$ ')
        q.send('Hi_TeSt'); q.expect('Please enter a string: ')
        q.send('disk hello'); q.expect('disk hello\r\n'); q.expect('$ ')
        q.send('fk_test'); q.expect('hello from pid:'); q.expect('hello from pid:')
        q.expect('$ ')
        q.send('ex_test'); q.expect('Please enter a string: ')
        q.send('nested exec'); q.expect('$ ')
        q.send('co_test')
        output = q.expect('step 49999 at pid', timeout=45)
        output += q.expect('step 49999 at pid', timeout=45)
        # The shell waits for its child, not that child's forked process.
        # Its prompt can arrive before the second process finishes.
        if b'$ ' not in output:
            q.expect('$ ')
        for _ in range(12):
            q.send('check.elf')
            q.expect('ELF data/BSS and failed exec preserved caller')
            q.expect('$ ')
        q.send('1'); q.expect('Command not found'); q.expect('$ ')
        q.send('missing'); q.expect('Command not found'); q.expect('$ ')
        q.send('bad.elf'); q.expect('Command not found'); q.expect('$ ')
    finally:
        q.close()
    print('QEMU: shell, hello, fork, exec, concurrency, BSS, repeated exec pass')

    # Change only the disk: hi_test must now execute the test ELF.
    sp.run(['mcopy', '-o', '-i', str(disk), str(elf), '::hi_test'], check=True)
    q = Qemu(disk)
    try:
        q.expect('$ '); q.send('hi_test')
        q.expect('ELF data/BSS and failed exec preserved caller')
    finally:
        q.close()
    print('QEMU: replacing a disk file changes the executed program')

    q = Qemu()
    try:
        q.expect('boot: cannot mount FAT16 disk')
    finally:
        q.close()
    original = disk.read_bytes()
    disk.write_bytes(bytes(len(original)))
    q = Qemu(disk)
    try:
        q.expect('boot: cannot mount FAT16 disk')
    finally:
        q.close()
    disk.write_bytes(original)
    sp.run(['mdel', '-i', str(disk), '::shell'], check=True)
    q = Qemu(disk)
    try:
        q.expect('boot: cannot load SHELL')
    finally:
        q.close()

    # Validate ELF metadata independently of FAT chain validation.
    source = bytearray(Path('user/shell').read_bytes())
    phoff = struct.unpack_from('<I', source, 28)[0]
    mutations = [(0, b'BAD!'), (4, b'\2'), (5, b'\2'), (18, b'\0\0'),
                 (28, b'\xff' * 4), (44, b'\xff\xff'),
                 (24, b'\xff' * 4), (phoff + 16, b'\xff' * 4),
                 (phoff + 20, b'\xff' * 4)]
    for offset, value in mutations:
        damaged = source.copy()
        damaged[offset:offset + len(value)] = value
        bad.write_bytes(damaged)
        sp.run(['mcopy', '-o', '-i', str(disk), str(bad), '::shell'],
               check=True)
        q = Qemu(disk)
        try:
            q.expect('boot: cannot load SHELL')
        finally:
            q.close()
    print('QEMU: missing disk/file, bad filesystem and malformed ELFs pass')
