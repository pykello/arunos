"""Serial-console helpers shared by QEMU tests."""
import os
import select
import subprocess as sp
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


