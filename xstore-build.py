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

ffi.cdef('typedef uint8_t u8;')
ffi.cdef('typedef unsigned int uint;')
ffi.cdef('typedef struct xhash_s xhash_s;')
ffi.cdef('void xcsum (const u8* restrict, const uint, u8* restrict);')
ffi.cdef('void xhash_iter (xhash_s* const restrict ctx, const u8* restrict, const uint);')
ffi.cdef('void xhash_done (xhash_s* const restrict ctx, const u8* restrict, const uint, void*, const uint);')
ffi.cdef('xhash_s* xhash_new (void);')
ffi.cdef('void xhash_free (xhash_s*);')

ffi.compile(target=('xstorelib.so'))

#
for f in ('xstorelib.c', 'xstorelib.o'):
    try:
        os.unlink(f)
    except FileNotFoundError:
        pass

# VERIFY
assert 64 <= os.stat('xstorelib.so').st_size
