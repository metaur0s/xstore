#!/usr/bin/python

import cffi
import xstorelib

class XHash:

    def __init__ (self):
        self.ctx = None
        self.ctx = xstorelib.lib.xhash_new()
        assert self.ctx # TODO: != NULL

    def __del__ (self):
        if self.ctx is not None:
            self.release()

    def put (self, value, size=None):
        assert self.ctx is not None
        assert isinstance(value, bytes)
        assert isinstance(size, int) or size is None
        if size is None:
            size = len(value)
        assert 0 <= size <= len(value)
        xstorelib.lib.xhash_iter(self.ctx, value, len(value))

    # TODO: RENAME done -> flush
    def done (self, value, size=None, hash_len=64):
        assert self.ctx is not None
        assert isinstance(value, bytes)
        assert isinstance(size, int) or size is None
        assert isinstance(hash_len, int) or hash_len is None
        if size is None:
            size = len(value)
        assert 0 <= size <= len(value)
        assert 1 <= hash_len <= 64
        hash = cffi.FFI().new('unsigned char [64]')
        xstorelib.lib.xhash_done(self.ctx, value, size, hash, hash_len)
        return bytes(hash)

    def reset (self):
        assert 1 == 2
        xstorelib.lib.xhash_reset(self.ctx)

    def release (self):
        assert self.ctx is not None
        self.ctx, ctx = None, self.ctx
        xstorelib.lib.xhash_free(ctx)

# VERIFY
hasher = XHash()
hasher.put(b'')
hasher.put(b'\x00')
assert hasher.done(b'') == b'\xed\xdd\x12\xb0\x0cp!\xf05\\tN1jU\xde\x86\xc5\xce\xf7i\x00\xc2\x97\xf2_\xd8\x16Ms\x17\xb8\xb2k\x0b\xeb_\xf9\xb2\xd2\n\xe6L\xb69\x8e\xb9\x01\xe5\x81A\xcb\x89\x9f\xf8\x15\x92s\xd2)T\xb9\xb3#'
hasher.release()
