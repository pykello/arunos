#!/usr/bin/env python3
"""Check header-triggered rebuild plans without changing source timestamps."""
import subprocess as sp

for header, outputs in [
    ('include/lib/syscall.h', ['lib/syscall.o', 'user/shell.o',
                              'kernel/ksyscall.o', 'disk.img.tmp']),
    ('include/proc.h', ['kernel/startup.o', 'kernel/proc/proc.o']),
    ('arch/versatilepb/linker.ld', ['arunos.elf']),
]:
    plan = sp.check_output(['make', '-n', '-W', header], text=True)
    for output in outputs:
        assert output in plan, (header, output, plan)
plan = sp.check_output(['make', '-n'], text=True)
assert 'arm-none-eabi-' not in plan and 'disk.img.tmp' not in plan, plan
print('build: headers trigger kernel, assembly, library and user rebuilds')
