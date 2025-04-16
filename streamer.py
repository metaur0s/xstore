#!/usr/bin/python

import os
import mmap
import time
from xhash import xhash64
import cbor2 as cbor
import zstandard

STREAMS_DIR = '/tmp/streams'

STREAM_ALIGNMENT = 4096

def _write_zip_internals (view, value, module, *what):
    exit(1)
    assert 1 == 2

# NOTE: ASSUME QUE DA EXCEPTION
# int(0xFFFF + 1).to_bytes(length=2, byteorder='little', signed=False)
# ASSIM NAO PRECISA FICAR CHECANDO OS VALORES

class StreamWriter:

    def __init__ (self):
        self.path = f'{STREAMS_DIR}/{int(time.time())}-{os.getpid()}'
        self.fd = -1
        self.mods = {}
        self.compressor = zstandard.ZstdCompressor(
            level=18,
            write_checksum=False,
            write_content_size=False,
            threads=8
        )

        self.fd = os.open(self.path, os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o0444)
        assert 0 <= self.fd <= 100

    def __del__ (self):
        if self.fd != -1:
            self.close()

    def close (self):
        fd = self.fd
        assert fd != -1
        self.fd = -1
        self.mods.clear()
        if size := os.lseek(fd, 0, os.SEEK_CUR):
            # TRUNCATE TO PAGE AND BLOCK SIZE
            # TODO: SÓ ADICIONAR ESSA MARCAO DE FIM SE O PAD RESULTANTE PELO TRUNCATE FOR AO MENOS 3
            # NAO É NECESSARIO POIS O TRUNCATE JA TERA 0
            # A FAKE NAME LENGTH 0
            #os.write(fd, b'\x00\x00\x00')
            aligned = ((size + STREAM_ALIGNMENT - 1) // STREAM_ALIGNMENT) * STREAM_ALIGNMENT
            assert 1 <= size <= aligned
            assert aligned % STREAM_ALIGNMENT == 0
            if aligned != size:
                os.truncate(fd, aligned)
            # SYNC
            assert not os.fsync(fd)
        else: # VAZIO, ENTAO APAGA
            os.unlink(self.path)
        assert not os.close(fd)

    def put (self, view, mod, what, orig):

        # TODO: when
        when = int(time.time())

        assert isinstance(view, str)
        assert isinstance(mod, str)
        assert isinstance(what, tuple)

        #
        if isinstance(orig, str):
            orig = orig.encode()
        elif isinstance(orig, (list, tuple, dict, set, str)) and len(orig) == 0:
            orig = b''
        elif isinstance(orig, (list, tuple, dict, set, str, float, int)):
            orig = cbor.dumps(orig)
        elif orig == b'PK\x05\x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00':
            orig = b'' # AN EMPTY ZIP ARCHIVE
        elif orig.startswith((b'PK\03\x04', b'MZ\xa1\x00')):
            _write_zip_internals(view, orig, mod, *what)
            orig = b'' # JUST TO BUMP IT
        else:
            # NOT A ZIP ON OTHER ENDIAN FORMAT :S
            # 0x04034b50
            assert not orig.startswith((b'PK', b'MZ')), orig[:256]

        #
        assert isinstance(orig, bytes)

        #
        try:
            hdr = self.mods[mod]
        except KeyError:
            hdr = b'\x00\x00'

            self.mods[mod] = (1 + len(self.mods)).to_bytes(byteorder='little', length=2, signed=False)

            # TEM QUE TER ESPAÇO PARA MAIS UM
            assert len(self.mods) <= 0xFFFF

            #
            mname_ = mod.encode()

            hdr += len(mname_).to_bytes(byteorder='little', length=1, signed=False)
            hdr += mname_

        #
        what = cbor.dumps(what)

        hdr += len(what).to_bytes(byteorder='little', length=2, signed=False)
        hdr += when     .to_bytes(byteorder='little', length=4, signed=False)
        hdr += what

        if orig:

            comp = self.compressor.compress(orig)

            hdr +=     len(orig).to_bytes(byteorder='little', length=4, signed=False)
            hdr +=     len(comp).to_bytes(byteorder='little', length=4, signed=False)
            hdr += xhash64(orig).to_bytes(byteorder='little', length=8, signed=False)
            hdr += xhash64(comp).to_bytes(byteorder='little', length=8, signed=False)
            hdr +=  xhash64(hdr).to_bytes(byteorder='little', length=8, signed=False)

            assert os.write(self.fd, hdr)  == len(hdr)
            assert os.write(self.fd, comp) == len(comp)

        else:
            # ORIG SIZE IS 0
            hdr += b'\x00\x00\x00\x00'
            hdr += xhash64(hdr).to_bytes(byteorder='little', length=8, signed=False)

            assert os.write(self.fd, hdr) == len(hdr)

class StreamReader:

    def __init__ (self, fpath):
        self.fd = -1
        self.fd = os.open(f'{STREAMS_DIR}/{sname}', os.O_RDONLY | os.O_DIRECT)
        # TODO: MMAP: sequential
        self.fmap = mmap.mmap(self.fd, os.lseek(self.fd, 0, os.SEEK_END), mmap.MAP_SHARED, mmap.PROT_READ, 0, 0)
        self.fmap.madvise(mmap.MADV_SEQUENTIAL)
        self.buff = memoryview(self.fmap)

    def __del__ (self):
        if self.fd != -1:
            self.close()

    def close (self):
        fd = self.fd
        assert fd != -1
        self.fd = -1
        self.buff.release()
        self.fmap.close()
        assert not os.close(fd)

#
myStream = StreamWriter()
for _ in range(10):
    myStream.put('TESTANDO / SEILA', 'SUPER|TESTE', ('symbol', 1, 'mas que coisa louca'), open('/tmp/test', 'r').read())
    myStream.put('TESTANDO / SEILA', 'SUPER|TESTE', ('symbol', 2,), open('/tmp/test', 'r').read())
    myStream.put('TESTANDO / SEILA', 'SUPER|TESTE', ('symbol', 3,), open('/tmp/test', 'r').read())
    myStream.put('TESTANDO / SEILA', 'SUPER|TESTE', ('symbol', 4,), open('/etc/fstab', 'r').read())
myStream.close()

for sname in sorted(os.listdir(STREAMS_DIR)):

    reader = StreamReader(sname)

    buff = reader.buff

    pos = 0

    mods = []

    while (pos + 3) <= len(buff):

        hdr_start = pos

        if buff[pos:pos+2] == b'\x00\x00':
            pos += 2
            size = int.from_bytes(buff[pos:pos+1], byteorder='little', signed=False)
            if size == 0:
                # FIM
                break
            pos += 1
            mod_id = len(mods)
            mods.append(bytes(buff[pos:pos+size]).decode())
            pos += size
        else:
            mod_id = int.from_bytes(buff[pos:pos+2], byteorder='little', signed=False) - 1
            assert mod_id >= 0
            pos += 2

        what_size = int.from_bytes(buff[pos:pos+2], byteorder='little', signed=False) ; pos += 2
        when      = int.from_bytes(buff[pos:pos+4], byteorder='little', signed=False) ; pos += 4
        what      =     cbor.loads(buff[pos:pos+what_size])                           ; pos += what_size
        orig_size = int.from_bytes(buff[pos:pos+4], byteorder='little', signed=False) ; pos += 4
        comp_size = int.from_bytes(buff[pos:pos+4], byteorder='little', signed=False) ; pos += 4
        orig_csum = int.from_bytes(buff[pos:pos+8], byteorder='little', signed=False) ; pos += 8
        comp_csum = int.from_bytes(buff[pos:pos+8], byteorder='little', signed=False) ; pos += 8
        hdr_end = pos
        hdr_csum  = int.from_bytes(buff[pos:pos+8], byteorder='little', signed=False) ; pos += 8

        print('WHEN %d HDR %016X COMP %016X ORIG %016X MOD [%s] SIZE %d' %
            (when, hdr_csum, comp_csum, orig_csum, mods[mod_id], orig_size), what)

        orig = zstandard.decompress(buff[pos:pos+comp_size], max_output_size=orig_size)

        assert len(orig) == orig_size

        assert xhash64(buff[hdr_start:hdr_end]) == hdr_csum
        assert xhash64(buff[pos:pos+comp_size]) == comp_csum
        assert xhash64(orig) == orig_csum

        pos += comp_size

        comp_csum = None

    buff = None

    reader.close()


um buffer single thread para criar os headers

o checksum64 inclui o header e o compressed
