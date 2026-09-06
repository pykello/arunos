#!/usr/bin/env python3
"""Exercise the kernel FAT reader against a real image and damaged copies."""
import ctypes as c
import os
from pathlib import Path
import struct
import subprocess as sp
import tempfile


class File(c.Structure):
    _fields_ = [('size', c.c_uint32), ('cluster', c.c_uint16)]


with tempfile.TemporaryDirectory() as tmp:
    tmp = Path(tmp)
    so = tmp / 'fat.so'
    sp.run(['gcc', '-shared', '-fPIC', '-fno-builtin', '-Iinclude',
            'kernel/fs/fat.c', 'lib/math.c', 'tests/fat_disk.c',
            '-o', str(so)], check=True)
    lib = c.CDLL(str(so))
    lib.fat_open.argtypes = [c.c_char_p, c.POINTER(File)]
    lib.fat_read.argtypes = [c.POINTER(File), c.c_uint32,
                             c.c_void_p, c.c_uint32]
    image = tmp / 'disk.img'
    image.write_bytes(Path('disk.img').read_bytes())
    fd = os.open(image, os.O_RDWR)
    lib.test_disk(fd, image.stat().st_size // 512)
    original = image.read_bytes()
    assert lib.fat_init()
    assert not lib.fat_open(b'MISSING ELF', c.byref(File()))
    for i, name in enumerate(['shell', 'hello', 'fork_test', 'exec_test',
                              'concurrency_test']):
        f = File()
        assert lib.fat_open(f'{i:<8}ELF'.encode(), c.byref(f))
        expected = Path('user', name).read_bytes()
        assert f.size == len(expected)
        for offset, length in [(0, f.size), (509, 1050),
                               (f.size - 7, 7), (f.size, 0)]:
            buf = c.create_string_buffer(length)
            assert lib.fat_read(c.byref(f), offset, buf, length)
            assert buf.raw == expected[offset:offset + length]
        assert not lib.fat_read(c.byref(f), f.size, c.create_string_buffer(1), 1)
    b = original[:512]
    reserved = struct.unpack_from('<H', b, 14)[0]
    fat_size = struct.unpack_from('<H', b, 22)[0]
    root = (reserved + b[16] * fat_size) * 512
    cluster = struct.unpack_from('<H', original, root + 26)[0]
    fat_offset = reserved * 512 + cluster * 2
    for offset, value in [(510, b'\0\0'), (11, b'\0\4'),
                          (13, b'\0'), (13, b'\3'), (22, b'\0\0'),
                          (32, b'\xff' * 4)]:
        # Total16 is nonzero on this image; clear it for Total32 testing.
        os.pwrite(fd, original[:512], 0)
        if offset == 32:
            os.pwrite(fd, b'\0\0', 19)
        os.pwrite(fd, value, offset)
        assert not lib.fat_init(), offset
    os.pwrite(fd, original[:512], 0)
    assert lib.fat_init()
    for next_cluster in [0, 1, 0xfff7, 0xffff, cluster]:
        os.pwrite(fd, struct.pack('<H', next_cluster), fat_offset)
        assert not lib.fat_open(b'0       ELF', c.byref(File()))
    os.close(fd)
    print('FAT: contents, unaligned reads, EOF, bad BPB and chains pass')
