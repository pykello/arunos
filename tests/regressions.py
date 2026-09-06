#!/usr/bin/env python3
"""Focused kernel regressions; optionally select a case by name."""
import sys
from pathlib import Path
import subprocess as sp
import tempfile
from qemu import Qemu


def early_input():
    for _ in range(3):
        q = Qemu('disk.img')
        try:
            q.send('early')
            q.expect('$ ')
            q.expect('Command not found')
            q.expect('$ ')
            q.send('ls')
            q.expect('HI_TEST')
            q.expect('$ ')
        finally:
            q.close()
    print('early input: boot and subsequent syscalls pass')


def fixture_disk(tmp, source):
    tmp = Path(tmp)
    obj, elf, disk = tmp / 'test.o', tmp / 'test.elf', tmp / 'disk.img'
    sp.run(['arm-none-eabi-gcc', '-mcpu=arm926ej-s', '-marm', '-fPIC',
            '-mapcs-frame', '-nostdinc', '-nostdlib', '-fno-builtin',
            '-Iinclude', '-c', source, '-o', str(obj)], check=True)
    sp.run(['arm-none-eabi-ld', '-Ttext=100', str(obj), 'lib/libarunos.a',
            '-o', str(elf)], check=True)
    disk.write_bytes(Path('disk.img').read_bytes())
    sp.run(['mcopy', '-o', '-i', str(disk), str(elf), '::shell'], check=True)
    return disk


def fork_limit():
    with tempfile.TemporaryDirectory() as tmp:
        q = Qemu(fixture_disk(tmp, 'tests/fork_limit.c'))
        try:
            q.expect('fork limit returned -1')
            # One wake character per child; space them to avoid UART overflow.
            import time
            for _ in range(9):
                q.p.stdin.write(b'x'); q.p.stdin.flush()
                time.sleep(0.05)
            q.expect('fork slots reusable')
        finally:
            q.close()
    print('fork limit: failure returns and freed slots are reusable')


CASES = {'early': early_input, 'fork': fork_limit}
if __name__ == '__main__':
    for name in sys.argv[1:] or CASES:
        CASES[name]()
