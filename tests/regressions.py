#!/usr/bin/env python3
"""Focused kernel regressions; optionally select a case by name."""
import sys
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


CASES = {'early': early_input}
if __name__ == '__main__':
    for name in sys.argv[1:] or CASES:
        CASES[name]()
