#!/usr/bin/python

import random
import time

def gen_binary ():

    G_N = 2

    WORDS_N = 8

    WORD_WIDTH = 64

    G = []

    for _ in range(G_N):

        bits = list('10' * (
            ((WORDS_N // G_N) * WORD_WIDTH)
                # METADE 1, METADE 0
                // 2
        ))

        random.shuffle(bits) ; time.sleep(0.35)
        random.shuffle(bits) ; time.sleep(0.35)
        random.shuffle(bits) ; time.sleep(0.35)

        G.append([
            bits[w * WORD_WIDTH:
                w * WORD_WIDTH + WORD_WIDTH]
            for w in range(len(bits) // WORD_WIDTH)
        ])

    for i in range(len(G[0])):
        for g in G:
                print('0b' + ''.join(g[i]) + 'ULL,')

def gen_hex ():

    G_N = 2

    WORDS_N = 64

    WORD_WIDTH = 8

    G = []

    for _ in range(G_N):

        bits = list('10' * (
            ((WORDS_N // G_N) * WORD_WIDTH)
                # METADE 1, METADE 0
                // 2
        ))

        random.shuffle(bits) ; time.sleep(0.35)
        random.shuffle(bits) ; time.sleep(0.35)
        random.shuffle(bits) ; time.sleep(0.35)

        G.append([
            bits[w * WORD_WIDTH:
                w * WORD_WIDTH + WORD_WIDTH]
            for w in range(len(bits) // WORD_WIDTH)
        ])

    print(', '.join('0x%02X' % int(''.join(g[i]), 2)
        for i in range(len(G[0]))
            for g in G
    ))

def gen_bytes ():

    for _ in range(8):

        N = list('01'*(512//2))

        random.shuffle(N)

        N = [ '0b' + ''.join(N[i*8:(i+1)*8]) for i in range(len(N)//8)]

        any( print(', '.join(N[i*8:(i+1)*8]) + (',' if i != 7 else '')) for i in range(len(N)//8) )

        print('---')

# gen_hex()



for _ in range(16):
    gen_binary()
    print('')
