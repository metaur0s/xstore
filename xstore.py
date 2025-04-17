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
        xstorelib.lib.xhash_iter(self.ctx, value, size)

    # TODO: RENAME done -> flush
    def done (self, value=None, size=None, hash_len=64):
        assert self.ctx is not None
        assert isinstance(value, bytes) or value is None
        assert isinstance(size, int) or size is None
        assert isinstance(hash_len, int) or hash_len is None
        if value is None:
            assert size is None
            # TODO: É PARA PARASSAR NONE -> NULL
            value, size = b'', 0
        elif size is None:
            size = len(value)
        assert 0 <= size <= len(value)
        assert 1 <= hash_len <= 64
        hash = cffi.FFI().new('unsigned char [64]')
        xstorelib.lib.xhash_done(self.ctx, value, size, hash, hash_len)
        hash = bytes(hash)
        assert len(hash) == hash_len
        print(hash)
        return hash

    def done_int(self, value=None, size=None, hash_len=64):
        hash = int.from_bytes(self.done(value, size, hash_len), byteorder='big', signed=False)
        print(hex(hash), value, size)
        return hash

    def reset (self):
        assert 1 == 2
        xstorelib.lib.xhash_reset(self.ctx)

    def release (self):
        assert self.ctx is not None
        self.ctx, ctx = None, self.ctx
        xstorelib.lib.xhash_free(ctx)

'''
# VERIFY
hasher = XHash()
assert hasher.done_int() == 0xf021700cb012ddedde556a314e745c3597c20069f7cec586b817734d16d85ff2d2b2f95feb0b6bb201b98e39b64ce60a15f89f89cb4181e523b3b95429d27392
assert hasher.done_int(b'\x00') == 0x1a1699b8d92cf8c72beb518c70d9ed3d83e84a796cf1194a12045ef4a7164f90ed51f94f557fd650a7b8b13972f99000ab528f413fe72738389fb93138ed5eac
assert hasher.done_int(b'\x00') == 0x2c8ea1bde188fe7a09c8ffc4d1e27867c3701ad7c973221a1d09b09bb48ee1ed790057e2365f4eadb4be3397f857ed11885e21e01ef4343d3da117b33e78b008
assert hasher.done_int(b'\x00\x01') == 0x2164bbfefbc814b727081d88a20e85ede40ebb0ae6bd4eba47274f9b3c64fcab878b8b0ea27e256b3bd47dcb058babe1c7b93c907a7bbb7e7ebb4afd54864e47

assert hasher.done(b'') == b"!d\xbb\xfe\xfb\xc8\x14\xb7'\x08\x1d\x88\xa2\x0e\x85\xed\xe4\x0e\xbb\n\xe6\xbdN\xbaG'O\x9b<d\xfc\xab\x87\x8b\x8b\x0e\xa2~%k;\xd4}\xcb\x05\x8b\xab\xe1\xc7\xb9<\x90z{\xbb~~\xbbJ\xfdT\x86NG"

hasher.release()


'''

assert XHash().done_int(b'12345678') == 0x49946744098c91100b89c120e6e8848979e7ff3c07337d6a5ecae191787905670ca02067817abb0b590711872933d976126406a0b4a82a964684e44be4bb447a
assert XHash().done_int(b'12345679') == 0x8002d30112ba1366c1590abba2cde7188153016f006d35668091cebbf23a01208096a7413f58630e8ede1b5e9180a56085d11366e2ef12986a0dcea67d2427bb
assert XHash().done_int(b'1234567A') == 0xed31fecced6fa8cac7cb76a0cf7357c9005a24cd3ceadb4d410552c3aa9209fab6b23fd8f8fb896bde455d2ed10c4f16c8f50a308c5054ad469bd82e097384d
assert XHash().done_int(b'1234567B') == 0xed31fecced6fa8cac7cb76a0cf7357c9005a24cd3ceadb4d410552c3aa9209fab6b23fd8f8fb896bde455d2ed10c4f16c8f50a308c5054ad469bd82e097384d

assert False
