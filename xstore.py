#!/usr/bin/python

import cffi
import xstorelib

_hash = cffi.FFI().new('unsigned char [64]')
