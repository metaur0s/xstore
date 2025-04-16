#!/usr/bin/python

import time
import xstore

#
TEST_BUFF_SIZE = 256*1024*1024

# SAMPLE DATA
sample = open('/dev/urandom', 'rb').read(TEST_BUFF_SIZE)

# BENCHMARK
for FUNC, func in (
    ('XCSUM', xstore.xstorelib.lib.xcsum),
    ('XHASH', xstore.xstorelib.lib.xhash),
):

    print(f'----- {FUNC}')

    for size, rounds in (
        ( 64,          5000000),
        (128,          5000000),
        (256,          5000000),
        (1024,         4000000),
        (65536,         120000),
        (256*1024,       50000),
        (64*1024*1024,     200),
        (128*1024*1024,    100),
    ):

        assert size <= TEST_BUFF_SIZE

        t = time.time()

        for _ in range (rounds):
            func(sample, size)

        t = time.time() - t

        ms = int(t * 1000)

        print('SIZE %9d ROUNDS %8d MS %5d MB/S %5d' %
            (size, rounds, ms, (size * rounds) / (t * 1024 * 1024)))

    print()
