
// TEST
#ifndef VERSE_BITS
#define VERSE_BITS 256
#endif

#ifndef XHASH_INVERT_ENDIANESS
#define XHASH_INVERT_ENDIANESS 0
#endif

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

#define ASSERT(c) ({ \
    if (!(c)) { \
        fprintf(stderr, "%s:%d: ASSERT FAILED: " #c "\n", __func__, __LINE__); \
        abort(); \
    } \
})

#define BUILD_ASSERT(c) _Static_assert(c, #c)

#define VERSE_WORDS (VERSE_BITS / (sizeof(xword_t) * 8))
#define VERSE_CHARS (VERSE_BITS / (sizeof(xchar_t) * 8))

//
#define WORD_CHARS (sizeof(xword_t) / sizeof(xchar_t))
#define WORD_BITS (sizeof(xword_t) * 8) // 0b111111U

typedef u64 xword_t;
typedef u8  xchar_t;

typedef xword_t words_v  __attribute__ ((vector_size(VERSE_WORDS * sizeof(xword_t))));
typedef xchar_t chars_v  __attribute__ ((vector_size(VERSE_CHARS * sizeof(xchar_t))));

typedef union verse_v {
    words_v w;
    chars_v c;
} verse_v;

// __attribute__((target("popcnt", "avx2")))

#if defined(__clang__)
#define __optimize // TODO:
#define __builtin_shuffle __builtin_shufflevector
#else
#define __optimize __attribute__((optimize("-O3", "-ffast-math", "-fstrict-aliasing")))
#endif

//
BUILD_ASSERT(sizeof(words_v)
          == sizeof(chars_v));

// THE TEMP SIZE
#define CTX_TSIZE(ctx) ((ctx)->tmp[sizeof(words_v) - 1])

// TODO: TEM QUE SER O NUMERO DE LINHA SOU O DE COLUNAS? OU O MAXIMO DE AMBOS?
#define V_LEN WORD_CHARS

typedef struct xhash_s {
    u8 tmp [sizeof(words_v)];
    u8 pad [sizeof(words_v)]; // PADDING
    verse_v v [V_LEN]; // SHUFFLER
} xhash_s;

#if VERSE_BITS == 512
#define __words(w0, w1, w2, w3, w4, w5, w6, w7) { w0, w1, w2, w3, w4, w5, w6, w7 }
#elif VERSE_BITS == 256
#define __words(w0, w1, w2, w3, w4, w5, w6, w7) { w0, w1, w2, w3 }
#elif VERSE_BITS == 128
#define __words(w0, w1, w2, w3, w4, w5, w6, w7) { w0, w1 }
#endif

#if VERSE_BITS == 512
#define __chars(                                 \
        c00, c01, c02, c03, c04, c05, c06, c07, \
        c08, c09, c10, c11, c12, c13, c14, c15, \
        c16, c17, c18, c19, c20, c21, c22, c23, \
        c24, c25, c26, c27, c28, c29, c30, c31, \
        c32, c33, c34, c35, c36, c37, c38, c39, \
        c40, c41, c42, c43, c44, c45, c46, c47, \
        c48, c49, c50, c51, c52, c53, c54, c55, \
        c56, c57, c58, c59, c60, c61, c62, c63  \
    ) {                                         \
        c00, c01, c02, c03, c04, c05, c06, c07, \
        c08, c09, c10, c11, c12, c13, c14, c15, \
        c16, c17, c18, c19, c20, c21, c22, c23, \
        c24, c25, c26, c27, c28, c29, c30, c31, \
        c32, c33, c34, c35, c36, c37, c38, c39, \
        c40, c41, c42, c43, c44, c45, c46, c47, \
        c48, c49, c50, c51, c52, c53, c54, c55, \
        c56, c57, c58, c59, c60, c61, c62, c63  \
    }
#elif VERSE_BITS == 256
#define __chars(                                 \
        c00, c01, c02, c03, c04, c05, c06, c07, \
        c08, c09, c10, c11, c12, c13, c14, c15, \
        c16, c17, c18, c19, c20, c21, c22, c23, \
        c24, c25, c26, c27, c28, c29, c30, c31, \
        c32, c33, c34, c35, c36, c37, c38, c39, \
        c40, c41, c42, c43, c44, c45, c46, c47, \
        c48, c49, c50, c51, c52, c53, c54, c55, \
        c56, c57, c58, c59, c60, c61, c62, c63  \
    ) {                                         \
        c00, c01, c02, c03, c04, c05, c06, c07, \
        c08, c09, c10, c11, c12, c13, c14, c15, \
        c16, c17, c18, c19, c20, c21, c22, c23, \
        c24, c25, c26, c27, c28, c29, c30, c31  \
    }
#elif VERSE_BITS == 128
#define __chars(                                 \
        c00, c01, c02, c03, c04, c05, c06, c07, \
        c08, c09, c10, c11, c12, c13, c14, c15, \
        c16, c17, c18, c19, c20, c21, c22, c23, \
        c24, c25, c26, c27, c28, c29, c30, c31, \
        c32, c33, c34, c35, c36, c37, c38, c39, \
        c40, c41, c42, c43, c44, c45, c46, c47, \
        c48, c49, c50, c51, c52, c53, c54, c55, \
        c56, c57, c58, c59, c60, c61, c62, c63  \
    ) {                                         \
        c00, c01, c02, c03, c04, c05, c06, c07, \
        c08, c09, c10, c11, c12, c13, c14, c15  \
    }
#endif

// TODO: PERMITIR INICIALIZAR OS PARAMETROS NO CREATE E NO RESET
static const xhash_s skel = {
    // TEMP, TEMP SIZE
    { 0 },

    __chars ( // PAD
        0x0B, 0xE9, 0x10, 0x9F, 0xC5, 0xC0, 0x99, 0x1E,
        0xA7, 0xEE, 0x56, 0xBC, 0xA6, 0xA4, 0x6A, 0xB3,
        0xE5, 0x21, 0x41, 0xD7, 0x92, 0xE5, 0xA9, 0xFE,
        0xAD, 0xB0, 0xFB, 0x6C, 0xD3, 0x46, 0xC2, 0x88,
        0x17, 0x8F, 0x35, 0xD4, 0x14, 0xB0, 0xFD, 0xD9,
        0x79, 0x46, 0x7C, 0x8B, 0x22, 0x18, 0x21, 0x4D,
        0x43, 0x1C, 0x1F, 0x33, 0x75, 0xD2, 0x4F, 0xC7,
        0x2B, 0xE0, 0x2E, 0x20, 0x27, 0x8A, 0x6A, 0xDF
    ),

    { // X
        __words (
            0b1010100110100010111000110011111110110110000100111100011101011101ULL,
            0b1000011111001101001111010111101100100110100000100101101101100110ULL,
            0b0101010011001110100101010111001101010010010010111100111110001101ULL,
            0b0011011100110101101001110100010010001101001000110101100101001011ULL,
            0b0100110111110110010110101001011100101001010000011000101000000001ULL,
            0b0010000001101101010010111110100110110100100110100010100101111101ULL,
            0b0101111111000011011010001101110000100000010111001101100101000100ULL,
            0b1000001100100001101101111010011011011110000001001000101111000111ULL
        ), __words (
            0b1111010001001011110111100001001111001110100100111100110110000000ULL,
            0b1000011110100111100010010111000000011111111000010101001010010101ULL,
            0b0001101011011111000100010000001110011001010110111011001010111010ULL,
            0b1010000110101100011101000110100111010001111101111100001011100110ULL,
            0b1011110111111110100100010110010101001100110100001010111101100000ULL,
            0b1101000011000100000111101001111100001101100100010100001100111111ULL,
            0b0101111110000001110001101100100110001100001001011101010001011000ULL,
            0b0100101011010001100100101010100011110011000101011101101110101100ULL
        ), __words (
            0b0101000100101010010010100000011010101011000101111101111101001101ULL,
            0b0111010101001111000111111100110000101000000000010100101101011001ULL,
            0b1111011101010100010101101101101111101101110110110001000001001100ULL,
            0b1001100101111100010111110000101010111111011000111011000111010111ULL,
            0b0000010100010000011000010001000000000001111000011010110011010111ULL,
            0b1100001001110011100101011101111100000001000010010111110011110110ULL,
            0b1111000000111101110010111101111011011101110111010110100110101000ULL,
            0b0101010000101000100101100111110100011000001001101000011101100100ULL
        ), __words (
            0b1110111000101100101010000100101011110010100010101100001110010010ULL,
            0b1111000111011101000111000110110000010011000110010010000101011101ULL,
            0b0001110000010011101110111100101110010001101001011110000101101010ULL,
            0b1000000101011011010000011011100101110011101010001000010001110101ULL,
            0b1000001010111110010010110011011110101010010010011101010101000100ULL,
            0b1100011111001001011101101001100111101010011010001010010110101001ULL,
            0b0011100011111000010011111000111110100000001111101110001011110110ULL,
            0b0001111101001110100000000111111010111101001100100101011110011001ULL
        ), __words (
            0b1001111111010111101100101101110010110110101001010110010111101001ULL,
            0b0001000011011010011100000010101100001101111100111101010001001111ULL,
            0b0101010111111010001010111010000100011000100000001011100110111110ULL,
            0b1011101111001010000101110001100110001000001110101110100101100001ULL,
            0b1011000101100110111111001000100100101101011001100111010001110100ULL,
            0b1110110001110101110111111100111101101010010111001010110000010000ULL,
            0b0111111100000100100010011110101010000001100010000010000101100100ULL,
            0b0110111000001110011011101001101010000000000100001110011101111100ULL
        ), __words (
            0b1010010010100011011110110001100101111000111000001001101110100101ULL,
            0b1111101001011111000110110010101100001101010011100010001110111111ULL,
            0b0111110100111111111011100000111001101001111100001010010110010001ULL,
            0b0101011000001001111011000011001110100100011110111110100111000000ULL,
            0b1010100111010101100000110110010100000101010110100001000001010010ULL,
            0b0001101001000011100000111111110101100000110100010011000011001100ULL,
            0b0101110111000110010011110000011111110001100010110010011011110100ULL,
            0b1011110111010110000001100101111000111010001111011000100010100001ULL
        ), __words (
            0b0001111101101010110011000001110000110111010111000101010001111000ULL,
            0b1101110001001000110110001010111111101001111000100000110001101001ULL,
            0b0100001111011101010100110011111000100010111101111100000100111000ULL,
            0b0110111000000010000001100010011000010111010110110011111010100010ULL,
            0b1100011001000101001100010111011101000110110001010110100010011100ULL,
            0b1000011010000001101110010001101011001100111010111100100011010000ULL,
            0b0110010100111001110111111000010100001011010110010100011001110011ULL,
            0b1001111101100100111101111110001110110001110101001110101101110001ULL
        ), __words (
            0b1111010101001001100101001100110011011000110010001101111011101111ULL,
            0b1001011100101110011001011111110001010010111111001000011010001011ULL,
            0b0010001111100011101011101100010011000110001001001000000000110101ULL,
            0b1001011000011011010011101011101010110001001010011000011111000010ULL,
            0b0000100101100110110101011011001101110001100100000111100011011101ULL,
            0b1101011001101001010001111010110001001110000101100000100111010110ULL,
            0b0011101111011100000101101100100010000001110010111010100011101111ULL,
            0b1111110100100001001101100010101110010100100000100101110011000111ULL
        )
    }
};

// NOTE: THE HASH IS SAVED BIG ENDIAN
// TODO: restrict ctx vs ctx->temp?
// TODO: UMA VERSAO ALINHADA
static inline void __optimize xhash_do (verse_v v[V_LEN], const u8* restrict data, uint q) {

    while (q) {
           q--;

        // ORIGINAL
        verse_v orig; memcpy(&orig, data, sizeof(verse_v)); data += sizeof(verse_v);

        // LOCAL ENDIANESS
#if XHASH_INVERT_ENDIANESS
        O.c = __builtin_shuffle( O.c,
            (chars_v) __chars (
                7,  6,  5,  4,  3,  2,  1,  0,
               15, 14, 13, 12, 11, 10,  9,  8,
               23, 22, 21, 20, 19, 18, 17, 16,
               31, 30, 29, 28, 27, 26, 25, 24,
               39, 38, 37, 36, 35, 34, 33, 32,
               47, 46, 45, 44, 43, 42, 41, 40,
               55, 54, 53, 52, 51, 50, 49, 48,
               63, 62, 61, 60, 59, 58, 57, 56
            )
        );
#endif

        // ACCUMULATE AND MIX
        for (uint i = 0; i != V_LEN; i++) {

            // SWAP BITS OF EVERY WORD
            verse_v shift = orig;

                                      // 1111111111111111111111111111111111111111111111111111111111111111|
            shift.w += shift.w >> 32; // 0000000000000000000000000000000011111111111111111111111111111111|11111111111111111111111111111111
            shift.w += shift.w >> 16; // 0000000000000000000000000000000000000000000000001111111111111111|1111111111111111
            shift.w += shift.w >>  8; // 0000000000000000000000000000000000000000000000000000000011111111|11111111
            shift.w += shift.w >>  4; // 0000000000000000000000000000000000000000000000000000000000001111|1111
            shift.w &= WORD_BITS - 1; // 0000000000000000000000000000000000000000000000000000000000111111|

            // USOU O ORIGINAL AGORA ALTERA ELE PARA O PROXIMO USO
            // O ORIGINAL NAO PERDE NENHUM BIT POIS NAO HA OVERFLOW
            orig.w = (orig.w >>              shift.w) |
                     (orig.w << (WORD_BITS - shift.w));

            //
            orig.c = __builtin_shuffle( orig.c,
                (chars_v) __chars (
                    // CADA LINHA TERA UM CARACTERE DE CADA LINHA, MAS MUDA TAMBEM O SEU CARACTERE DE LUGAR
                    // [ print(', '.join( '%2d' % lines[c][i] for c in range(8) )) for i in range(8)  ]
                    // [ print(', '.join(sorted([ '%2d' % lines[c][i] for c in range(8) ], reverse=True))) for i in range(8)  ]
                    56, 48, 40, 32, 24, 16,  8, 0,
                    57, 49, 41, 33, 25, 17,  9, 1,
                    58, 50, 42, 34, 26, 18, 10, 2,
                    59, 51, 43, 35, 27, 19, 11, 3,
                    60, 52, 44, 36, 28, 20, 12, 4,
                    61, 53, 45, 37, 29, 21, 13, 5,
                    62, 54, 46, 38, 30, 22, 14, 6,
                    63, 55, 47, 39, 31, 23, 15, 7
                )
            );

            // TODO: ESQUECE ESSE A
            v[i].w += orig.w;

            //
            v[i].w += (words_v) __builtin_shuffle(
                // OPOSITE INDEX
                // [ (i, (8 - 1) - i) for i in range(8) ]
                //      -> [(0, 7), (1, 6), (2, 5), (3, 4), (4, 3), (5, 2), (6, 1), (7, 0)]
                v[(V_LEN - 1) - i].c,
                // USE A AS A MAP, TO EXTRACT FROM THE OPOSITE INDEX
                v[i].c & (sizeof(chars_v) - 1)
            );
        }
    }
}

void __optimize xhash_put (xhash_s* const restrict ctx, const u8* restrict data, uint size) {

    // NOTE: É SAFE PASSAR DATA NULL COM SIZE 0
    ASSERT(data != NULL || size == 0);

    ASSERT(CTX_TSIZE(ctx) < sizeof(words_v));

    // QUANTO TEM
    uint tsize = CTX_TSIZE(ctx);

    if (tsize) {

        ASSERT(tsize < sizeof(words_v));

        // QUANTO FALTA PARA COMPLETAR
        uint puxar = sizeof(words_v) - tsize;

        // SÓ PODE PEGAR O QUE TEM
        if (puxar > size)
            puxar = size;

        memcpy(ctx->tmp + tsize, data, puxar);

        // TIROU DO BUFFER...
        data += puxar;
        size -= puxar;

        // ...E COLOCOU NO TEMP
        if ((tsize += puxar) != sizeof(words_v)) {
            // AINDA NAO TEM UM TEMP COMPLETO
            ASSERT(size == 0);
            CTX_TSIZE(ctx) = tsize;
            return;
        }

        xhash_do(ctx->v, ctx->tmp, 1);
    }

    // NAO TEM TEMP
    xhash_do(ctx->v, data, size / sizeof(words_v));

    // SOBROU ISSO
    if ((tsize = size % sizeof(words_v)))
        memcpy(ctx->tmp, data + size - tsize, tsize);

    CTX_TSIZE(ctx) = tsize;
}

void __optimize xhash_flush (xhash_s* const restrict ctx, const u8* restrict data, uint size, u8* const restrict hash, const uint hash_len) {

    ASSERT(data != NULL || size == 0);
    ASSERT(hash != NULL || hash_len == 0);

    ASSERT(CTX_TSIZE(ctx) < sizeof(words_v));

    // ????????
    if (hash_len > sizeof(verse_v))
        // TODO: FAILED
        return;

    //
    xhash_put(ctx, data, size);

    if (hash_len) {

        //
        const xhash_s backup = *ctx;

        // O PUT() CONSUMIU TODO O DATA/SIZE
        ASSERT(CTX_TSIZE(ctx) < sizeof(words_v));

        // NA VERDADE NÃO É SÓ UM PAD, VAI USAR ELE INTEIRO MESMO QUE NAO TENHA TEMP
        const uint tsize = CTX_TSIZE(ctx);

        // PAD
        // NOTE QUE AQUI ESTÁ OVERWRITING TSIZE...
        // ...WITH ITSELF
        memset(ctx->tmp + tsize, tsize, sizeof(words_v) - tsize);

        BUILD_ASSERT((offsetof(xhash_s, tmp) + sizeof(words_v))
                   == offsetof(xhash_s, pad));

        // TMP + PAD + TRAIL + V
        xhash_do(ctx->v, ctx->tmp, (1 + 1 + V_LEN - 1)); // TODO: POR CAUSA DO RESTRICT

        //
        verse_v result = ctx->v[V_LEN - 1]; // TODO: RESULT TEM QUE TER OTAMANHO >= hash_len!!!!

        // __builtin_rotateright

        // TODO: REDUCE ALL WORDS?

        // GLOBAL ENDIANESS
#if XHASH_INVERT_ENDIANESS
        result.c = __builtin_shuffle( result.c,
            (chars_v) __chars (
                7,  6,  5,  4,  3,  2,  1,  0,
               15, 14, 13, 12, 11, 10,  9,  8,
               23, 22, 21, 20, 19, 18, 17, 16,
               31, 30, 29, 28, 27, 26, 25, 24,
               39, 38, 37, 36, 35, 34, 33, 32,
               47, 46, 45, 44, 43, 42, 41, 40,
               55, 54, 53, 52, 51, 50, 49, 48,
               63, 62, 61, 60, 59, 58, 57, 56
            )
        );
#endif

#if XHASH_INVERT_ENDIANESS
        ASSERT(BE64(ctx->v.w[0]) == result.w[0]);
        ASSERT(BE64(ctx->v.w[1]) == result.w[1]);
#endif

        // PASS
        memcpy(hash, &result, hash_len);

        //
        *ctx = backup;

        ASSERT(CTX_TSIZE(ctx) == tsize);
    }
}

// REINITIALIZE
void xhash_reset (xhash_s* const ctx) {

    memcpy(ctx, &skel, sizeof(xhash_s));

    ASSERT(CTX_TSIZE(ctx) == 0);
}

xhash_s* xhash_new (void) {

    xhash_s* const ctx = aligned_alloc(sizeof(words_v), sizeof(xhash_s));

    if (ctx) {
        // INITIALIZE

        xhash_reset(ctx);
    }

    return ctx;
}

void xhash_free (xhash_s* const ctx) {

    ASSERT(CTX_TSIZE(ctx) < sizeof(words_v));

    free(ctx);
}

// FOR SPEED
// WE CAN ALWAYS DO A EVEN DEEPER CHECK BY VERIFYING THE HASH OF THE FILES
void __optimize xcsum (const void* restrict data, uint size, void* restrict csum);
