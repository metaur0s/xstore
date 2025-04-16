
#include <stdint.h>
#include <string.h>

typedef unsigned int uint;

typedef uint8_t   u8;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint64_t u64;

#define popcount64 __builtin_popcountll

// TODO:
#if 1
#define BE64 __builtin_bswap64
#else
#define BE64
#endif
#define BE8

static inline u64 swap64q (const u64 x, const uint q) {

    return (x >> q) | (x << (64 - q));
}

static inline u64 swap64 (const u64 x) {

    return swap64q(x, popcount64(x));
}

typedef u64 v512 __attribute__ ((vector_size(8 * sizeof(u64))));
typedef u64 v256 __attribute__ ((vector_size(4 * sizeof(u64))));

v512 hash;
v256 csum;

// __attribute__((target("popcnt", "avx2")))

// NOTE: THE HASH IS SAVED BIG ENDIAN
void __attribute__((optimize("-O3", "-ffast-math", "-fstrict-aliasing"))) xhash (const void* restrict data, uint size) {

    // ACUMULATORS
    // import random, time ; N = list('10' * ((4 * 64) // 2)) ; random.shuffle(N) ; time.sleep(0.2) ; random.shuffle(N) ; any(print('0b' + ''.join(N[w*64:w*64+64]) + 'ULL,') for w in range(len(N) // 64))
    v512 A = {
        0b1110000110010001000101100101000110101011100001010001101011100011ULL,
        0b0001101110000110111010000111100011000001101101001110101001000110ULL,
        0b0000101000011010010001111011101100011000110001001100111111111100ULL,
        0b0111111111101111011001100100101100010001100110110100110010111001ULL,
        0b1010010011011110001000000111111011001010101110100100010111000110ULL,
        0b0000101011100111011010000000000110011001110010010100000100111100ULL,
        0b1111101011011111100100110010111010010001110111110111001000100011ULL,
        0b1001110100110001000011010000101100000111001011001111101111100101ULL,
    };

    // SWAPPERS
    // SIZE DEPENDENT
    u64 x0 = 0b0101010101010101010101010101010101010101010101010101010101010101ULL * size;
    u64 x1 = 0b0000000100000001000000010000000100000001000000010000000100000001ULL * size;
    u64 x2 = 0b1010101010101010101010101010101010101010101010101010101010101010ULL * size;
    u64 x3 = 0b0001000100010001000100010001000100010001000100010001000100010001ULL * size;

    while (size) {

        u64 w;

        if (size >= sizeof(u64)) {
            size -= sizeof(u64);
            w = BE64(*(u64*)data);
            data += sizeof(u64);
        } else {
            size -= sizeof(u8);
            w = BE8(*(u8*)data);
            data += sizeof(u8);
        }

        // STATUS VS VALUE
        x0 = swap64(w += x3);
        x1 = swap64(w += x2);
        x2 = swap64(w += x1);
        x3 = swap64(w += x0);

        // ACCUMULATE
        // A IDÉIA É QUE CADA WORD DO VETOR VAI SE ALTERANDO DE FORMA DIFERENTE
        A *= w += (x0 + x1) * x2;
        A += w *= (x1 + x2) * x3;
        A *= w += (x2 + x3) * x0;
        A += w *= (x3 + x0) * x1;
    }

    // ALL WORDS CONTAIN THE INFO
    A += A[7];
    A ^= A[6];
    A += A[5];
    A ^= A[4];
    A += A[3];
    A ^= A[2];
    A += A[1];

    // SAVE
    // WARNING: ENDIANESS
    hash = A;
}

// FOR SPEED
// WE CAN ALWAYS DO A EVEN DEEPER CHECK BY VERIFYING THE HASH OF THE FILES
void __attribute__((optimize("-O3", "-ffast-math", "-fstrict-aliasing"))) xcsum (const void* restrict data, uint size) {

    v256 A = {
        0b1110000110010001000101100101000110101011100001010001101011100011ULL,
        0b0001101110000110111010000111100011000001101101001110101001000110ULL,
        0b0000101000011010010001111011101100011000110001001100111111111100ULL,
        0b0111111111101111011001100100101100010001100110110100110010111001ULL,
    };

    u64 x0 = 0b0101010101010101010101010101010101010101010101010101010101010101ULL;
    u64 x1 = 0b1010101010101010101010101010101010101010101010101010101010101010ULL;

    while (size) {

        u64 w;

        if (size >= sizeof(u64)) {
            size -= sizeof(u64);
            w = BE64(*(u64*)data);
            data += sizeof(u64);
        } else {
            size -= sizeof(u8);
            w = BE8(*(u8*)data);
            data += sizeof(u8);
        }

        A += x0 ^= w;
        A ^= x1 += w;
    }

    // ALL WORDS CONTAIN THE INFO
    A += A[3];
    A ^= A[2];
    A += A[1];

    // SAVE
    csum = A;
}
