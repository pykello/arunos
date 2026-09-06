#!/usr/bin/env python3
"""Exercise two-stage initialization using the real allocator."""
from pathlib import Path
import subprocess as sp
import tempfile

with tempfile.TemporaryDirectory() as tmp:
    exe = str(Path(tmp) / 'allocator')
    # Keep static pages below 4 GiB to match the kernel's address type.
    sp.run(['gcc', '-no-pie', '-g', '-fsanitize=address,undefined',
            '-Wno-int-to-pointer-cast', '-Iinclude',
            '-Iarch/versatilepb/include', 'tests/allocator.c',
            'kernel/memory/kalloc.c', '-o', exe], check=True)
    sp.run([exe], check=True)
    print('allocator: both free lists survive adding a second region')
