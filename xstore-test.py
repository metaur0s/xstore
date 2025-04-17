#!/usr/bin/python

import time
import cffi
from xstore import XHash

#
TEST_BUFF_SIZE = 256*1024*1024

# SAMPLE DATA
sample = open('/dev/urandom', 'rb').read(TEST_BUFF_SIZE)

for i in range(100):
    s = 10 + i
    print(('%0128X' % XHash().done_int(sample, s), s, s%8, ))
exit(0)
hasher = XHash()

# BENCHMARK
for FUNC, func in (
    #('XCSUM', xstore.xstorelib.lib.xcsum),
    ('XHASH', hasher.put),
):

    print(f'----- {FUNC}')

    for size, rounds in (
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

        t = time.time()

        for _ in range (rounds):
            func(sample, size)

        t = time.time() - t

        ms = int(t * 1000)

        print('SIZE %9d ROUNDS %8d MS %5d MB/S %5d' %
            (size, rounds, ms, (size * rounds) / (t * 1024 * 1024)))

    print()

'''

ANTES DO CTX
SIZE        64 ROUNDS  4000000 MS  4503 MB/S    54
SIZE       128 ROUNDS  4000000 MS  6301 MB/S    77
SIZE       256 ROUNDS  2800000 MS  6809 MB/S   100
SIZE      1024 ROUNDS   500000 MS  3784 MB/S   129
SIZE      8192 ROUNDS    80000 MS  4444 MB/S   140
SIZE    262144 ROUNDS     2500 MS  4379 MB/S   142
SIZE    524288 ROUNDS     1000 MS  3503 MB/S   142
SIZE   8388608 ROUNDS       25 MS  1400 MB/S   142

12:27:23 xtrader ~/xstore $ python^C
12:53:31 xtrader ~/xstore $ python xstore-build.py && python xstore-test.py
----- XHASH
SIZE        64 ROUNDS  4000000 MS  3265 MB/S    74
SIZE       128 ROUNDS  4000000 MS  5039 MB/S    96
SIZE       256 ROUNDS  2800000 MS  5875 MB/S   116
SIZE      1024 ROUNDS   500000 MS  3554 MB/S   137
SIZE      8192 ROUNDS    80000 MS  4324 MB/S   144
SIZE    262144 ROUNDS     2500 MS  4286 MB/S   145
SIZE    524288 ROUNDS     1000 MS  3427 MB/S   145
SIZE   8388608 ROUNDS       25 MS  1370 MB/S   145

'''
