#!/usr/bin/python

import cffi
import xstorelib

class XHash:

    B = 0

    HASH_LEN = 0

    _new      = None
    _put      = None
    _flush    = None
    _reset    = None
    _free     = None

    def __init__ (self):
        self.ctx = None
        self.ctx = self._new()
        assert self.ctx # TODO: != NULL
        self._hash = cffi.FFI().new('unsigned char [64]')

    def __del__ (self):
        if self.ctx is not None:
            self.release()

    # TODO: SUPORTAR UM ARGUMENTO "start" PARA NÃO PRECISAR USAR []
    def put (self, value, size=None):

        assert self.ctx is not None

        assert isinstance(value, bytes)
        assert isinstance(size, int) or size is None

        if size is None:
            size = len(value)

        assert 0 <= size <= len(value)

        self._put(self.ctx, value, size)

    def flush (self, value=None, size=None, hash_len=None):

        assert self.ctx is not None

        assert isinstance(value, bytes) or value is None
        assert isinstance(size, int) or size is None
        assert isinstance(hash_len, int) or hash_len is None

        if value is None:
            assert size is None
            # TODO: É PARA PASSAR NONE -> NULL
            value, size = b'', 0
        elif size is None:
            size = len(value)

        if hash_len is None:
            hash_len = self.HASH_LEN

        assert isinstance(size, int)
        assert isinstance(hash_len, int)

        assert 0 <= size <= len(value)
        assert 0 <= hash_len <= self.HASH_LEN

        self._flush(self.ctx, value, size, self._hash, hash_len)

        hash = bytes(self._hash)[:hash_len]

        assert len(hash) == hash_len
        # print(hash)

        return hash

    def flush_int (self, value=None, size=None, hash_len=None):

        assert self.ctx is not None

        hash = int.from_bytes(self.flush(value, size, hash_len), byteorder='big', signed=False)

        #print(hex(hash), value, size)

        return hash

    def reset (self):

        assert self.ctx is not None

        self._reset(self.ctx)

    def release (self):

        assert self.ctx is not None

        self.ctx, ctx = None, self.ctx
        self._free(ctx)

class XHash512 (XHash):

    B = 512

    HASH_LEN = 64

    _new      = xstorelib.lib.xhash512_new
    _put      = xstorelib.lib.xhash512_put
    _flush    = xstorelib.lib.xhash512_flush
    _reset    = xstorelib.lib.xhash512_reset
    _free     = xstorelib.lib.xhash512_free

class XHash256 (XHash):

    B = 256

    HASH_LEN = 32

    _new      = xstorelib.lib.xhash256_new
    _put      = xstorelib.lib.xhash256_put
    _flush    = xstorelib.lib.xhash256_flush
    _reset    = xstorelib.lib.xhash256_reset
    _free     = xstorelib.lib.xhash256_free

class XHash128 (XHash):

    B = 128

    HASH_LEN = 16

    _new      = xstorelib.lib.xhash128_new
    _put      = xstorelib.lib.xhash128_put
    _flush    = xstorelib.lib.xhash128_flush
    _reset    = xstorelib.lib.xhash128_reset
    _free     = xstorelib.lib.xhash128_free

'''
# VERIFY
hasher = XHash()
assert hasher.flush_int() == 0xf021700cb012ddedde556a314e745c3597c20069f7cec586b817734d16d85ff2d2b2f95feb0b6bb201b98e39b64ce60a15f89f89cb4181e523b3b95429d27392
assert hasher.flush_int(b'\x00') == 0x1a1699b8d92cf8c72beb518c70d9ed3d83e84a796cf1194a12045ef4a7164f90ed51f94f557fd650a7b8b13972f99000ab528f413fe72738389fb93138ed5eac
assert hasher.flush_int(b'\x00') == 0x2c8ea1bde188fe7a09c8ffc4d1e27867c3701ad7c973221a1d09b09bb48ee1ed790057e2365f4eadb4be3397f857ed11885e21e01ef4343d3da117b33e78b008
assert hasher.flush_int(b'\x00\x01') == 0x2164bbfefbc814b727081d88a20e85ede40ebb0ae6bd4eba47274f9b3c64fcab878b8b0ea27e256b3bd47dcb058babe1c7b93c907a7bbb7e7ebb4afd54864e47

assert hasher.flush(b'') == b"!d\xbb\xfe\xfb\xc8\x14\xb7'\x08\x1d\x88\xa2\x0e\x85\xed\xe4\x0e\xbb\n\xe6\xbdN\xbaG'O\x9b<d\xfc\xab\x87\x8b\x8b\x0e\xa2~%k;\xd4}\xcb\x05\x8b\xab\xe1\xc7\xb9<\x90z{\xbb~~\xbbJ\xfdT\x86NG"

hasher.release()


assert XHash().flush_int(b'12345670') == 0x8f8f0ddeee978d9a827f0c6e85ff3e1487c4d4dfd094ed16839f8f7f4a839b591f4ff556f9eebf1fcfdc9f157f2f138dff1b9f74f694ce4ad23d9a3e201e7ddd
assert XHash().flush_int(b'12345671') == 0x3380b36565d4cea5b3a565ef265213f540e5e48ee5d651a5bf792c24f91f7f761f63cdb45385d45c0d54cf24331514cf757823258fdc185362f863fe5f1737e4
assert XHash().flush_int(b'12345672') == 0x1c929c70dd1209b17ead05618a4a87acecdcb1c68fe4ca6fd348a8761732091c5267b10f4949ca0b280bc9da1d64535de8d03e1fd59c5c6d1193ac103198aa57
assert XHash().flush_int(b'12345673') == 0x1b7e8ee0ce46cfba9510f3ec4e2eb37482f23f9bd430ab60d50f55c30f031fd91bdcda028e0988dbbf5560fe30163f8def62c2f28d067f74da7e473dec1e555d
assert XHash().flush_int(b'12345674') == 0x294cb909f8fcefefe6a46f3e2b295564c2a9ee5715fc7be2c04995ca19e954aa154f7c1e017496201e6f21a4ed56fa62ef16784aa667277bb808e4eca33cc93e
assert XHash().flush_int(b'12345675') == 0x5edfa783ca9b4def0f30042fce0c8b117a01172a375af1c11479d6b22fccce0e28a96520d198125ae0360db64cf8171bf1502d32168b1cb0b914cb51cedb2f9a
assert XHash().flush_int(b'12345678') == 0xe7836762257ce7beca9b6f9d680a549022a7d2b2576860c0ea92c83b8d19fdd8134860aaa6e6989834a3e9e9f3a6e5091b644a67c7c0a63b3006a6641929182f
assert XHash().flush_int(b'12345679') == 0x399cb960f9b729938152c09ca1349d4586b71f76d321277adf9977b118cb74e5f8339c2e8702773aeec1b20774cffa762817b4d9b6d977a6b69cd97044bac47a
assert XHash().flush_int(b'1234567A') == 0x296938b978a27417508880d0911aa97d10c0e3d0c507e6b2e41f2433f814f4592a641214b6b8d063a94517d9af14e4005dda5fc11c792a699bc4e9f77590241d
assert XHash().flush_int(b'1234567B') == 0x274be969dd125eb65dd409b4ce6da993fa9bf6b0b72e27229bfa541f9d4ed1a30ca96ddb51525ddc28120d92651cd18974c49ddb95de1c8f386c5d62e62e446d
assert XHash().flush_int(b'1234567C') == 0xe782e562257de7bbca9ecc9d680a549422a75b72576862c0ea92c83c8d19fdd8134909aaa6e6989834a3e7a9f3a6e5091b640127c7c0a63cefc728641929182f
assert XHash().flush_int(b'1234567D') == 0x3169d7d2cfd7b054725b137ed83329647972c76ad60c96ed693ed49752906f576e2d5416ff6607176a3d0fccef0cad22b23bba44c82d6b96ca2dd816178d2595
assert XHash().flush_int(b'1234567E') == 0x695a58ed294e5418ba35050ed37b2ff4176dba36b94dfcc0ba33589d7057d35f6a3e555784fd1d1ff8dc145c58058e4031dd4ae93cbbdfa0f92a0020d70e56cd
assert XHash().flush_int(b'1234567F') == 0xd5c9cf8a0fe550ceb8be96ed92aae1fa89feabd2e57e97c61be4ae0eb60f3b47d66ebcb9feff64d2914c8725360a30a2e27c64bf09aa30b6b2d7fd0d80e225a1

'''

def TEST ():

    def add (x):
        A.append(x)
        return x

    test_data = open('/dev/urandom', 'rb').read(256*1024)

    assert len(test_data) == 256*1024

    # TEST
    for XHash, B in ((XHash128, 128), (XHash256, 256), (XHash512, 512)):

        print(XHash, B)

        assert XHash.B == B
        assert XHash.HASH_LEN == B // 8

        assert len(XHash().flush()) == XHash.HASH_LEN

        for i in range(XHash.HASH_LEN):
            assert len(XHash().flush(hash_len=i)) == i

        for i in range(64):

            hasher, A = XHash(), []

            # MULTIPLE TIMES, TO REUSE THE SAME CONTEXT
            for _ in range(4):

                hasher.reset() ; A.clear()

                hasher.put(add(b''))
                hasher.put(add(test_data[i*20:
                                         i*20 + 24]))
                hasher.put(add(test_data[i*45 + 28:
                                         i*45 + 28 + 228]))

                hash1 = hasher.flush(hash_len=10)

                # RODAR DE NOVO TEM QUE DAR NO MESMO
                assert hash1 == hasher.flush(hash_len=10), 'REPEATED-FLUSH-MISMATCH'
                assert hash1 == hasher.flush(hash_len=10), 'REPEATED-FLUSH-MISMATCH'
                assert hash1 == hasher.flush(b'IGNORED-IGNORED-IGNORED-IGNORED', 0, hash_len=10), 'REPEATED-FLUSH-MISMATCH'
                assert hash1 == hasher.flush(hash_len=10), 'REPEATED-FLUSH-MISMATCH'

                # TEM QUE RESULTAR NO MESMO QUE RODANDO TUDO DE UMA VEZ SÓ
                for _ in range(10):

                    assert hash1 == XHash().flush(b''.join(A), hash_len=10), 'MULTIPLE-VS-SINGLE-MISMATCH'

                # REPETIR TEM QUE DAR IGUAL
                for _ in range(10):

                    hasher2 = XHash()

                    for a in A:
                        hasher2.put(a)

                    assert hash1 == hasher2.flush(hash_len=10), 'REPEATED-MISMATCH'

                # CONTINUING AFTER DONE VS CONTINUING WITHOUT DONE
                # TODO: PARA QUE ISSO FUNCIONE, nao pode destruir o tmp
                assert hash1 == XHash().flush(b''.join(A), hash_len=10)

            hasher.release()

            del hasher

if __name__ == '__main__':

    TEST()


#assert XHash512().flush_int(b'ewfwegwegw') == 3896899742622206537538947522840705516870972400341980601150199566990789049151764991401302434553606248169300087694932825121597483481462325205314209420511206

