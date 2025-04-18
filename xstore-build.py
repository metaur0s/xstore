#!/usr/bin

import os

from cffi import FFI

#
for f in (
    'xstore.o',
    'xstore.so',
    'xstorelib.c',
    'xstorelib.o',
    'xstorelib.so',
):
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

# clang  -Wall -Wextra -Wno-unused-label -Wno-error=unused-function -Wno-error=unused-variable -Werror -Wfatal-errors -fstrict-aliasing -std=gnu11 -fdiagnostics-color=auto -O2 -march=native -mtune=native -c

ffi.cdef('typedef uint8_t u8;')
ffi.cdef('typedef unsigned int uint;')

for B in (128, 256, 512):
    ffi.cdef(f'typedef struct xhash{B}_s xhash{B}_s;')
    ffi.cdef(f'void xhash{B}_put   (xhash{B}_s* const restrict ctx, const u8* restrict, const uint);')
    ffi.cdef(f'void xhash{B}_flush (xhash{B}_s* const restrict ctx, const u8* restrict, const uint, void*, const uint);')
    ffi.cdef(f'void xhash{B}_reset (xhash{B}_s*);')
    ffi.cdef(f'void xhash{B}_free (xhash{B}_s*);')
    ffi.cdef(f'xhash{B}_s* xhash{B}_new (void);')

ffi.compile(target=('xstorelib.so'))

#
for f in (
    'xstore.o',
    'xstore.so',
    'xstorelib.c',
    'xstorelib.o',
):
    try:
        os.unlink(f)
    except FileNotFoundError:
        pass

# VERIFY
assert 64 <= os.stat('xstorelib.so').st_size
