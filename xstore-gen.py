#!/usr/bin/python

import random
import time

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
