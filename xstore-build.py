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

ffi.cdef('typedef struct ctx_s ctx_s;')
ffi.cdef('void xcsum (const unsigned char* restrict, const unsigned int, unsigned char* restrict);')
ffi.cdef('void xhash_iter (ctx_s* const restrict ctx, const unsigned char* restrict, const unsigned int);')
ffi.cdef('void xhash_done (ctx_s* const restrict ctx, const unsigned char* restrict, const unsigned int, unsigned char* restrict);')
ffi.cdef('uintptr_t hash_new (void);')

ffi.compile(target=('xstorelib.so'))

#
for f in ('xstorelib.c', 'xstorelib.o'):
    try:
        os.unlink(f)
    except FileNotFoundError:
        pass

# VERIFY
assert 64 <= os.stat('xstorelib.so').st_size
