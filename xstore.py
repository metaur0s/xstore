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
        hash = bytes(hash)[:hash_len]
        assert len(hash) == hash_len
        # print(hash)
        return hash

    def done_int(self, value=None, size=None, hash_len=64):
        hash = int.from_bytes(self.done(value, size, hash_len), byteorder='big', signed=False)
        #print(hex(hash), value, size)
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


assert XHash().done_int(b'12345670') == 0x8f8f0ddeee978d9a827f0c6e85ff3e1487c4d4dfd094ed16839f8f7f4a839b591f4ff556f9eebf1fcfdc9f157f2f138dff1b9f74f694ce4ad23d9a3e201e7ddd
assert XHash().done_int(b'12345671') == 0x3380b36565d4cea5b3a565ef265213f540e5e48ee5d651a5bf792c24f91f7f761f63cdb45385d45c0d54cf24331514cf757823258fdc185362f863fe5f1737e4
assert XHash().done_int(b'12345672') == 0x1c929c70dd1209b17ead05618a4a87acecdcb1c68fe4ca6fd348a8761732091c5267b10f4949ca0b280bc9da1d64535de8d03e1fd59c5c6d1193ac103198aa57
assert XHash().done_int(b'12345673') == 0x1b7e8ee0ce46cfba9510f3ec4e2eb37482f23f9bd430ab60d50f55c30f031fd91bdcda028e0988dbbf5560fe30163f8def62c2f28d067f74da7e473dec1e555d
assert XHash().done_int(b'12345674') == 0x294cb909f8fcefefe6a46f3e2b295564c2a9ee5715fc7be2c04995ca19e954aa154f7c1e017496201e6f21a4ed56fa62ef16784aa667277bb808e4eca33cc93e
assert XHash().done_int(b'12345675') == 0x5edfa783ca9b4def0f30042fce0c8b117a01172a375af1c11479d6b22fccce0e28a96520d198125ae0360db64cf8171bf1502d32168b1cb0b914cb51cedb2f9a
assert XHash().done_int(b'12345678') == 0xe7836762257ce7beca9b6f9d680a549022a7d2b2576860c0ea92c83b8d19fdd8134860aaa6e6989834a3e9e9f3a6e5091b644a67c7c0a63b3006a6641929182f
assert XHash().done_int(b'12345679') == 0x399cb960f9b729938152c09ca1349d4586b71f76d321277adf9977b118cb74e5f8339c2e8702773aeec1b20774cffa762817b4d9b6d977a6b69cd97044bac47a
assert XHash().done_int(b'1234567A') == 0x296938b978a27417508880d0911aa97d10c0e3d0c507e6b2e41f2433f814f4592a641214b6b8d063a94517d9af14e4005dda5fc11c792a699bc4e9f77590241d
assert XHash().done_int(b'1234567B') == 0x274be969dd125eb65dd409b4ce6da993fa9bf6b0b72e27229bfa541f9d4ed1a30ca96ddb51525ddc28120d92651cd18974c49ddb95de1c8f386c5d62e62e446d
assert XHash().done_int(b'1234567C') == 0xe782e562257de7bbca9ecc9d680a549422a75b72576862c0ea92c83c8d19fdd8134909aaa6e6989834a3e7a9f3a6e5091b640127c7c0a63cefc728641929182f
assert XHash().done_int(b'1234567D') == 0x3169d7d2cfd7b054725b137ed83329647972c76ad60c96ed693ed49752906f576e2d5416ff6607176a3d0fccef0cad22b23bba44c82d6b96ca2dd816178d2595
assert XHash().done_int(b'1234567E') == 0x695a58ed294e5418ba35050ed37b2ff4176dba36b94dfcc0ba33589d7057d35f6a3e555784fd1d1ff8dc145c58058e4031dd4ae93cbbdfa0f92a0020d70e56cd
assert XHash().done_int(b'1234567F') == 0xd5c9cf8a0fe550ceb8be96ed92aae1fa89feabd2e57e97c61be4ae0eb60f3b47d66ebcb9feff64d2914c8725360a30a2e27c64bf09aa30b6b2d7fd0d80e225a1

'''

# TEST
for i in range(64):
    hasher = XHash()
    hasher.put(b'2F9476AC6BDC0F3BA348493D2951469A87519C1D721E7C33A655DDE6809EA0C0004F42339964E789B0AF0E1A9D7F0000C06A0F1A9D7F0000B0AF0E1A9D7F0000')
    hasher.put(b'2f9476ac6bdc0f3ba348493d2951469a87519c1d721e7c33a655dde6809ea0c0004f42339964e789b0af0e1a9d7f0000c06a0f1a9d7f0000b0af0e1a9d7f0000')
    hasher.put(b'2F9476AC6BDC0F3BA348493D2951469A87519C1D721E7C33A655DDE6809EA0C0004F42339964E789B0AF0E1A9D7F0000C06A0F1A9D7F0000B0AF0E1A9D7F0000'[11:11+i])
    assert hasher.done(hash_len=10) == XHash().done(
        b'2F9476AC6BDC0F3BA348493D2951469A87519C1D721E7C33A655DDE6809EA0C0004F42339964E789B0AF0E1A9D7F0000C06A0F1A9D7F0000B0AF0E1A9D7F0000' +
        b'2f9476ac6bdc0f3ba348493d2951469a87519c1d721e7c33a655dde6809ea0c0004f42339964e789b0af0e1a9d7f0000c06a0f1a9d7f0000b0af0e1a9d7f0000' +
        b'2F9476AC6BDC0F3BA348493D2951469A87519C1D721E7C33A655DDE6809EA0C0004F42339964E789B0AF0E1A9D7F0000C06A0F1A9D7F0000B0AF0E1A9D7F0000'[11:11+i]
        , hash_len=10)
    hasher.release()
    del hasher
