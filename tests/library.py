#!/usr/bin/env python3
"""Run the actual C library with address and undefined-behavior sanitizers."""
from pathlib import Path
import subprocess as sp
import tempfile

with tempfile.TemporaryDirectory() as tmp:
    exe = str(Path(tmp) / 'library')
    # Avoid replacing host libc symbols needed by the sanitizer runtime.
    symbols = ['memcpy', 'memset', 'strlen', 'strcpy', 'strcmp', 'strncmp',
               'strchr', 'strtok', 'strlcpy']
    sp.run(['gcc', '-g', '-fno-builtin', '-fsanitize=address,undefined',
            '-Iinclude', *['-D%s=ar_%s' % (s, s) for s in symbols],
            'tests/library.c', 'lib/string.c', 'lib/stdio/gets.c',
            '-o', exe], check=True)
    sp.run([exe], check=True)
    print('library: bounded line input passes with sanitizers')
