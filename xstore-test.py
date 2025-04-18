#!/usr/bin/python

import time

from xstore import XHash128, XHash256, XHash512

#
TEST_BUFF_SIZE = 256*1024*1024

# SAMPLE DATA
sample = open('/dev/urandom', 'rb').read(TEST_BUFF_SIZE)

for XHash in (XHash128, XHash256, XHash512):

    for i in range(100):
        s = 10 + i
        print(XHash, (f'%0{(XHash.B//8) * 2}x' % XHash().flush_int(sample, s), s, s%8, ))

    print()

# BENCHMARK
for XHash in (XHash128, XHash256, XHash512):

    print(XHash)

    hasher = XHash()

    for size, rounds in (
        (  8,            20000),
        ( 16,            20000),

        (  8,          5000000),
        ( 16,          5000000),
        ( 64,          4000000),
        (128,          4000000),
        (256,          2800000),
        (1024,          500000),
        (8192,           80000),
        (256*1024,        2500),
        (512*1024,        1000),
        (8*1024*1024,       25),
    ):

        assert size <= TEST_BUFF_SIZE

        hasher.reset()

        t = time.time()

        for _ in range (rounds):
            #hasher.flush(sample, size)
            XHash._flush(hasher.ctx, sample, size, hasher._hash, 0)

        t = time.time() - t

        ms = int(t * 1000)

        print('SIZE %9d ROUNDS %8d MS %5d MB/S %5d' %
            (size, rounds, ms, (size * rounds) / (t * 1024 * 1024)))

    print()
