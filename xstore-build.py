#!/usr/bin

import os

from cffi import FFI

#
for f in ('xstorelib.c', 'xstorelib.o', 'xstorelib.so'):
    try:
        os.unlink(f)
    except FileNotFoundError:
        pass

ffi = FFI()

ffi.set_source("xstorelib", open('xstore.c').read(), libraries=[],
    extra_compile_args=[
        "-std=gnu11",
        "-Wall",
        "-Wextra",
        "-Werror",
        "-Wfatal-errors",
        "-O2",
#        "-ffast-math",
        "-march=native",
       # "-fstrict-aliasing"
    ]
)

ffi.cdef('void xcsum (const unsigned char* restrict, const unsigned int, unsigned char*);')
ffi.cdef('void xhash (const unsigned char* restrict, const unsigned int, unsigned char*);')

ffi.compile(target=('xstorelib.so'))

#
for f in ('xstorelib.c', 'xstorelib.o'):
    try:
        os.unlink(f)
    except FileNotFoundError:
        pass

# VERIFY
assert 64 <= os.stat('xstorelib.so').st_size
