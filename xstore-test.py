#!/usr/bin/python

import time
import xstore

#
TEST_BUFF_SIZE = 256*1024*1024

# SAMPLE DATA
sample = open('/dev/urandom', 'rb').read(TEST_BUFF_SIZE)

# BENCHMARK
for FUNC, func in (
    ('XHASH', xstore.xstorelib.lib.xhash),
    ('XCSUM', xstore.xstorelib.lib.xcsum),
):

    print(f'----- {FUNC}')

    for size, rounds in (
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

        t = time.time()

        for _ in range (rounds):
            func(sample, size)

        t = time.time() - t

        ms = int(t * 1000)

        print('SIZE %9d ROUNDS %8d MS %5d MB/S %5d' %
            (size, rounds, ms, (size * rounds) / (t * 1024 * 1024)))

    print()
