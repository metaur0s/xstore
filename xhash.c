// CONFIG

#define XHASH_INVERT_ENDIANESS 1

#define DO_O_ENDIANESS         R_LOOP   (O_ENDIANESS)
#define DO_O_ROTATE_CHARS_BITS R_LOOP   (O_ROTATE_BITS)
#define DO_O_ROTATE_CHARS      R_LOOP   (O_ROTATE_BITS)
#define DO_A_ACCUM             R_INLINE (A_ADD_O)
#define DO_A_MIX               R_INLINE (A_MIX)
#define DO_A_ENDIANESS         R_LOOP   (A_ENDIANESS)

#include <stddef.h> // offsetof
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // DEBUG / ASSERT

typedef unsigned int uint;

typedef uint8_t   u8;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint64_t u64;

// TODO:
#if 1
#define BE64 __builtin_bswap64
#else
#define BE64
#endif

// TODO: __builtin_expect
// !SUCESSO -> 0
#define ASSERT(c) ({ \
    if (__builtin_expect_with_probability(!(c), 0, 0.99)) { \
        fprintf(stderr, "%s:%d: ASSERT FAILED: " #c "\n", __func__, __LINE__); \
        abort(); \
    } \
})

#define BUILD_ASSERT(c) _Static_assert(c, #c)

// __attribute__((target("popcnt", "avx2")))

#if defined(__clang__)
#define __optimize // TODO:
#define __builtin_shuffle __builtin_shufflevector
#else
#define __optimize __attribute__((optimize("-O3", "-ffast-math", "-fstrict-aliasing")))
#endif

//
#define XALIGNMENT 64

//
#define RBITS  256 // ON A REGISTER
#define WBITS   64 // ON A WORD
#define CBITS    8 // ON A CHAR

#define REGISTERS_N 16

#define REGISTER_WORDS_N (RBITS / WBITS)
#define REGISTER_CHARS_N (RBITS / CBITS)

typedef u64 xword_t;
typedef u8  xchar_t;

typedef xword_t xregister_w __attribute__ ((vector_size(REGISTER_WORDS_N * sizeof(xword_t))));
typedef xchar_t xregister_c __attribute__ ((vector_size(REGISTER_CHARS_N * sizeof(xchar_t))));

typedef union xregister {
    xregister_w w; // QUEBRADO EM PEDACOS DE MAXIMO TAMANHO
    xregister_c c; // QUEBRADO EM PEDACOS DE MINIMO TAMANHO
} xregister;

//
BUILD_ASSERT(sizeof(xword_t)   * 8 == WBITS);
BUILD_ASSERT(sizeof(xchar_t)   * 8 == CBITS);
BUILD_ASSERT(sizeof(xregister) * 8 == RBITS);

//
BUILD_ASSERT(sizeof(xregister_w) == sizeof(xword_t) * REGISTER_WORDS_N);
BUILD_ASSERT(sizeof(xregister_c) == sizeof(xchar_t) * REGISTER_CHARS_N);

//
BUILD_ASSERT(sizeof(xregister) == sizeof(xregister_w));
BUILD_ASSERT(sizeof(xregister) == sizeof(xregister_c));

#define ASSERT_CTX_TSIZE(ctx) ASSERT((ctx)->tsize < sizeof((ctx)->tmp))
#define ASSERT_CTX_LOOPS(ctx) ASSERT(0 < (ctx)->loops && (ctx)->loops <= 1000)

typedef struct xhash_s {
    u8          tmp [REGISTERS_N * sizeof(xregister)];
    u8          pad [REGISTERS_N * sizeof(xregister)]; // PADDING (USEFUL TO FORCE A FINAL MIXING)
    xregister   acc [REGISTERS_N]; // ACCUMULATOR
    xregister_c rot; // ROTATOR
    xregister_c end; // ENDIANESS
    u16 tsize;
    u16 loops;
    u32 reserved;
    u64 total;
} xhash_s;

static const xhash_s skel = {

    .tmp = { 0 },

    .pad = {
        0 // TODO:
    },

    .acc = {
        {{ 0b1010100110100010111000110011111110110110000100111100011101011101ULL, 0b1000011111001101001111010111101100100110100000100101101101100110ULL, 0b0101010011001110100101010111001101010010010010111100111110001101ULL, 0b0011011100110101101001110100010010001101001000110101100101001011ULL }},
        {{ 0b0100110111110110010110101001011100101001010000011000101000000001ULL, 0b0010000001101101010010111110100110110100100110100010100101111101ULL, 0b0101111111000011011010001101110000100000010111001101100101000100ULL, 0b1000001100100001101101111010011011011110000001001000101111000111ULL }},
        {{ 0b1111010001001011110111100001001111001110100100111100110110000000ULL, 0b1000011110100111100010010111000000011111111000010101001010010101ULL, 0b0001101011011111000100010000001110011001010110111011001010111010ULL, 0b1010000110101100011101000110100111010001111101111100001011100110ULL }},
        {{ 0b1011110111111110100100010110010101001100110100001010111101100000ULL, 0b1101000011000100000111101001111100001101100100010100001100111111ULL, 0b0101111110000001110001101100100110001100001001011101010001011000ULL, 0b0100101011010001100100101010100011110011000101011101101110101100ULL }},
        {{ 0b0101000100101010010010100000011010101011000101111101111101001101ULL, 0b0111010101001111000111111100110000101000000000010100101101011001ULL, 0b1111011101010100010101101101101111101101110110110001000001001100ULL, 0b1001100101111100010111110000101010111111011000111011000111010111ULL }},
        {{ 0b0000010100010000011000010001000000000001111000011010110011010111ULL, 0b1100001001110011100101011101111100000001000010010111110011110110ULL, 0b1111000000111101110010111101111011011101110111010110100110101000ULL, 0b0101010000101000100101100111110100011000001001101000011101100100ULL }},
        {{ 0b1110111000101100101010000100101011110010100010101100001110010010ULL, 0b1111000111011101000111000110110000010011000110010010000101011101ULL, 0b0001110000010011101110111100101110010001101001011110000101101010ULL, 0b1000000101011011010000011011100101110011101010001000010001110101ULL }},
        {{ 0b1000001010111110010010110011011110101010010010011101010101000100ULL, 0b1100011111001001011101101001100111101010011010001010010110101001ULL, 0b0011100011111000010011111000111110100000001111101110001011110110ULL, 0b0001111101001110100000000111111010111101001100100101011110011001ULL }},
        {{ 0b1001111111010111101100101101110010110110101001010110010111101001ULL, 0b0001000011011010011100000010101100001101111100111101010001001111ULL, 0b0101010111111010001010111010000100011000100000001011100110111110ULL, 0b1011101111001010000101110001100110001000001110101110100101100001ULL }},
        {{ 0b1011000101100110111111001000100100101101011001100111010001110100ULL, 0b1110110001110101110111111100111101101010010111001010110000010000ULL, 0b0111111100000100100010011110101010000001100010000010000101100100ULL, 0b0110111000001110011011101001101010000000000100001110011101111100ULL }},
        {{ 0b1010010010100011011110110001100101111000111000001001101110100101ULL, 0b1111101001011111000110110010101100001101010011100010001110111111ULL, 0b0111110100111111111011100000111001101001111100001010010110010001ULL, 0b0101011000001001111011000011001110100100011110111110100111000000ULL }},
        {{ 0b1010100111010101100000110110010100000101010110100001000001010010ULL, 0b0001101001000011100000111111110101100000110100010011000011001100ULL, 0b0101110111000110010011110000011111110001100010110010011011110100ULL, 0b1011110111010110000001100101111000111010001111011000100010100001ULL }},
        {{ 0b0001111101101010110011000001110000110111010111000101010001111000ULL, 0b1101110001001000110110001010111111101001111000100000110001101001ULL, 0b0100001111011101010100110011111000100010111101111100000100111000ULL, 0b0110111000000010000001100010011000010111010110110011111010100010ULL }},
        {{ 0b1100011001000101001100010111011101000110110001010110100010011100ULL, 0b1000011010000001101110010001101011001100111010111100100011010000ULL, 0b0110010100111001110111111000010100001011010110010100011001110011ULL, 0b1001111101100100111101111110001110110001110101001110101101110001ULL }},
        {{ 0b1111010101001001100101001100110011011000110010001101111011101111ULL, 0b1001011100101110011001011111110001010010111111001000011010001011ULL, 0b0010001111100011101011101100010011000110001001001000000000110101ULL, 0b1001011000011011010011101011101010110001001010011000011111000010ULL }},
        {{ 0b0000100101100110110101011011001101110001100100000111100011011101ULL, 0b1101011001101001010001111010110001001110000101100000100111010110ULL, 0b0011101111011100000101101100100010000001110010111010100011101111ULL, 0b1111110100100001001101100010101110010100100000100101110011000111ULL }}
    },

    // LINE X COLS
    //
    // SHIFT ARRAY
    // any( print(',   '.join([ ', '.join([ '%3d' % (line*4*8 + word*8 + c + 1) for c in range(8) ]) for word in range(4)  ]) + ',') for line in range(16) )
    // SHIFT CADA REGISTRO DE FORMA DIFERENTE
    // REG_N=16 ; REG_CHARS=32 ; any( print( '{ ' + ', '.join([ '%2d' % ((r*(REG_CHARS // 2 + 2) + 1 + c) % REG_CHARS) for c in range(REG_CHARS) ]) + ' },' ) for r in range(REG_N) )
    //
    .rot = {
        1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,  0
    },

    // ENDIANESS
    .end = { // [ print(' '.join([ ', '.join([ '%3d' % ( (line*32) +   8*l + (8 - 1 - i)) for i in range(8) ]) + ',  '  for l in range(4) ])) for line in range(16) ]
        7, 6, 5, 4, 3, 2, 1, 0,   15, 14, 13, 12, 11, 10, 9, 8,   23, 22, 21, 20, 19, 18, 17, 16,   31, 30, 29, 28, 27, 26, 25, 24
    },

    .tsize = 0,
    .loops = 8,
    .reserved = 0,
    .total = 0,
};

//                                                                       |---------------------------------------------------------------|
//                                                              |----------------------------------------------------------------|
//                                                   |-------------------------------------------------------------------|
//                                         |---------------------------------------------------------------------|
//                                |----------------------------------------------------------------------|
//                      |------------------------------------------------------------------------|
//            |--------------------------------------------------------------------------|
//   |---------------------------------------------------------------------------|
// [(8, 15), (9, 14), (10, 13), (11, 12), (12, 11), (13, 10), (14, 9), (15, 8), (0, 7), (1, 6), (2, 5), (3, 4), (4, 3), (5, 2), (6, 1), (7, 0)]
//       |----------------------------------------------------------------------------------------------------------------------------------|
//                |-----------------------------------------------------------------------------------------------------------------|
//                          |-----------------------------------------------------------------------------------------------|
//                                    |-----------------------------------------------------------------------------|
//                                              |-----------------------------------------------------------|
//                                                        |-----------------------------------------|
//                                                                 |------------------------|
//                                                                          |-------|
//     0       1          2        3        4          5          6       7        8      9       10       11     12      13      14      15
// N = 16 ; [ ((i + N//2) % N, ((N - 1) - i)) for i in range(N) ]
#define INDEX_HALFWAY(n, i) (((i) + (n)/2) % (n))
#define INDEX_OPOSITE(n, i) (((n) - 1) - (i))

#define O_ROTATE_BITS(r) \
     orig[r].w = \
    (orig[r].w >> (WBITS - (r + 1))) | \
    (orig[r].w << (        (r + 1)))

#define O_ROTATE_CHARS(r)  orig[r].c = __builtin_shuffle(     orig[r].c, ctx->rot[r])
#define O_ENDIANESS(r)     orig[r].c = __builtin_shuffle(     orig[r].c, ctx->end )
#define A_ENDIANESS(r) ctx->acc[r].c = __builtin_shuffle( ctx->acc[r].c, ctx->end )
#define A_ADD_O(r)     ctx->acc[r].w += orig[r].w

#define A_MIX(r) \
    ctx->acc[r].w += \
        (xregister_w) __builtin_shuffle ( \
            ctx->acc[INDEX_HALFWAY(REGISTERS_N, r)].c, \
            ctx->acc[INDEX_OPOSITE(REGISTERS_N, r)].c & (REGISTER_CHARS_N - 1) \
        )

#if REGISTERS_N == 32
#define R_INLINE(OP)                                                    \
    {                                                                   \
        OP( 0); OP( 1); OP( 2); OP( 3); OP( 4); OP( 5); OP( 6); OP( 7); \
        OP( 8); OP( 9); OP(10); OP(11); OP(12); OP(13); OP(14); OP(15); \
        OP(16); OP(17); OP(18); OP(19); OP(20); OP(21); OP(22); OP(23); \
        OP(24); OP(25); OP(26); OP(27); OP(28); OP(29); OP(30); OP(31); \
    }
#elif REGISTERS_N == 16
#define R_INLINE(OP)                                                    \
    {                                                                   \
        OP( 0); OP( 1); OP( 2); OP( 3); OP( 4); OP( 5); OP( 6); OP( 7); \
        OP( 8); OP( 9); OP(10); OP(11); OP(12); OP(13); OP(14); OP(15); \
    }
#elif REGISTERS_N == 8
#define R_INLINE(OP)                                                    \
    {                                                                   \
        OP( 0); OP( 1); OP( 2); OP( 3); OP( 4); OP( 5); OP( 6); OP( 7); \
    }
#endif

#define R_LOOP(OP) { for (uint r = 0; r != REGISTERS_N; r++) { OP(r); }}

// NOTE: THE HASH IS SAVED BIG ENDIAN
// TODO: restrict ctx vs ctx->temp?
// TODO: UMA VERSAO ALINHADA (SEM PRECISAR DO MEMCPY?)
// TODO: UMA VERSAO ALINHADA E OPTIMIZADA, DESTRUTIVEL DAA
static inline void __optimize xhash_do (xhash_s* const ctx, const u8* restrict data, uint q) {

    ASSERT_CTX_TSIZE(ctx);
    ASSERT_CTX_LOOPS(ctx);

    while (q) {
           q--;

        // ORIGINAL
        xregister orig [REGISTERS_N]; memcpy(&orig, data, sizeof(orig)); data += sizeof(orig);

        // LOCAL ENDIANESS
        DO_O_ENDIANESS;

        uint loops = ctx->loops;

        do {

            // ACCUMULATE ORIGINAL BITS IN THE RESPECTIVE REGISTERS
            DO_A_ACCUM;
            // MIX
            // EACH REGISTER USE ANOTHER ONE AS A MAP, TO EXTRACT CHARACTERS FROM ANOTHER ONE
            DO_A_MIX;

            // ROTATE BITS, PER WORD, IN EACH REGISTER
            DO_O_ROTATE_CHARS_BITS;
            // ROTATE BYTES, BETWEEN WORDS, IN EACH REGISTER
            DO_O_ROTATE_CHARS;

        } while (--loops);
    }
}

void __optimize xhash_put (xhash_s* const restrict ctx, const u8* restrict data, uint size) {

    // NOTE: É SAFE PASSAR DATA NULL COM SIZE 0
    ASSERT(data != NULL || size == 0);

    ASSERT_CTX_TSIZE(ctx);
    ASSERT_CTX_LOOPS(ctx);

    // QUANTO TEM
    uint tsize = ctx->tsize;

    if (tsize) {

        // QUANTO FALTA PARA COMPLETAR
        uint puxar = sizeof(ctx->tmp) - tsize;

        // SÓ PODE PEGAR O QUE TEM
        if (puxar > size)
            puxar = size;

        memcpy(ctx->tmp + tsize, data, puxar);

        // TIROU DO BUFFER...
        data += puxar;
        size -= puxar;

        // ...E COLOCOU NO TEMP
        if ((tsize += puxar) != sizeof(ctx->tmp)) {
            // AINDA NAO TEM UM TEMP COMPLETO
            ASSERT(size == 0);
            ctx->tsize = tsize;
            return;
        }

        xhash_do(ctx, ctx->tmp, 1);
    }

    // NAO TEM TEMP
    xhash_do(ctx, data, size / sizeof(ctx->tmp));

    // SOBROU ISSO
    if ((tsize = size % sizeof(ctx->tmp)))
        memcpy(ctx->tmp, data + size - tsize, tsize);

    ctx->tsize = tsize;
}

void __optimize xhash_flush (xhash_s* const restrict ctx, const u8* restrict data, uint size, u8* const restrict hash, const uint hash_len) {

    ASSERT(data != NULL || size == 0);
    ASSERT(hash != NULL || hash_len == 0);

    ASSERT_CTX_TSIZE(ctx);
    ASSERT_CTX_LOOPS(ctx);

    // ????????
    if (hash_len > sizeof(xregister))
        // TODO: FAILED
        return;

    //
    xhash_put(ctx, data, size);

    if (hash_len) { // <-- ESTA CERTO ISSO?

        //
        const xhash_s backup = *ctx;

        // FLUSH ANY REMAINING TEMP
        const uint tsize = ctx->tsize;

        if (tsize) {

            // PAD
            memcpy(ctx->tmp  + tsize,
                   ctx->pad  + tsize,
            sizeof(ctx->tmp) - tsize);

            xhash_do(ctx, ctx->tmp, 1);
        }

        // GLOBAL ENDIANESS
        DO_A_ENDIANESS;

        // PASS
        memcpy(hash, &ctx->acc, hash_len);

        //
        *ctx = backup;

        ASSERT(ctx->tsize == tsize);
    }
}

// REINITIALIZE
void xhash_reset (xhash_s* const ctx) {

    memcpy(ctx, &skel, sizeof(xhash_s));

    ASSERT(ctx->tsize == 0);
}

xhash_s* xhash_new (void) {

    xhash_s* const ctx = aligned_alloc(XALIGNMENT, sizeof(xhash_s));

    if (ctx) {
        // INITIALIZE

        xhash_reset(ctx);
    }

    ASSERT_CTX_TSIZE(ctx);
    ASSERT_CTX_LOOPS(ctx);

    return ctx;
}

void xhash_free (xhash_s* const ctx) {

    ASSERT_CTX_TSIZE(ctx);
    ASSERT_CTX_LOOPS(ctx);

    free(ctx);
}

// FOR SPEED
// WE CAN ALWAYS DO A EVEN DEEPER CHECK BY VERIFYING THE HASH OF THE FILES
void __optimize xcsum (const void* restrict data, uint size, void* restrict csum);
