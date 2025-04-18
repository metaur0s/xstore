
// TEST
#ifndef XHASH_BITS
#define XHASH_BITS 256
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // DEBUG / ASSERT

typedef unsigned int uint;
typedef unsigned long long int uintll;

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
    //} else {  fprintf(stderr, "OK @ %s:%d\n", __func__, __LINE__); }

#define BUILD_ASSERT(c) _Static_assert(c, #c)

// __attribute__((target("popcnt", "avx2")))

#if XHASH_BITS == 512
typedef u64 u64x8  __attribute__ ((vector_size( 8 * sizeof(u64))));
typedef u32 u32x16 __attribute__ ((vector_size(16 * sizeof(u32))));
typedef u16 u16x32 __attribute__ ((vector_size(32 * sizeof(u16))));
typedef u8  u8x64  __attribute__ ((vector_size(64 * sizeof(u8))));
#elif XHASH_BITS == 256
typedef u64 u64x4  __attribute__ ((vector_size( 4 * sizeof(u64))));
typedef u32 u32x8  __attribute__ ((vector_size( 8 * sizeof(u32))));
typedef u16 u16x16 __attribute__ ((vector_size(16 * sizeof(u16))));
typedef u8  u8x32  __attribute__ ((vector_size(32 * sizeof(u8))));
#elif XHASH_BITS == 128
typedef u64 u64x2  __attribute__ ((vector_size( 2 * sizeof(u64))));
typedef u32 u32x4  __attribute__ ((vector_size( 4 * sizeof(u32))));
typedef u16 u16x8  __attribute__ ((vector_size( 8 * sizeof(u16))));
typedef u8  u8x16  __attribute__ ((vector_size(16 * sizeof(u8))));
#endif

#if defined(__clang__)
#define __optimize // TODO:
#define __builtin_shuffle __builtin_shufflevector
#else
#define __optimize __attribute__((optimize("-O3", "-ffast-math", "-fstrict-aliasing")))
#endif

#define X_LEN 8

#if XHASH_BITS == 512
typedef u64x8 xhash_t;
typedef u8x64 xhash_bytes_t;
#elif XHASH_BITS == 256
typedef u64x4 xhash_t;
typedef u8x32 xhash_bytes_t;
#elif XHASH_BITS == 128
typedef u64x2 xhash_t;
typedef u8x16 xhash_bytes_t;
#endif

//
BUILD_ASSERT(sizeof(xhash_t)
          == sizeof(xhash_bytes_t));

// THE TEMP SIZE
#define CTX_TSIZE(ctx) ((ctx)->tmp[sizeof(xhash_t) - 1])

typedef struct xhash_s {
    xhash_t X [X_LEN]; // SHUFFLER
    xhash_t A; // ACCUMULATOR
    u8 tmp [sizeof(xhash_t)];
    u8 pad [sizeof(xhash_t)]; // PADDING
} xhash_s;

#if XHASH_BITS == 512
#define __v_8(x0, x1, x2, x3, x4, x5, x6, x7) { x0, x1, x2, x3, x4, x5, x6, x7 }
#elif XHASH_BITS == 256
#define __v_8(x0, x1, x2, x3, x4, x5, x6, x7) { x0, x1, x2, x3 }
#elif XHASH_BITS == 128
#define __v_8(x0, x1, x2, x3, x4, x5, x6, x7) { x0, x1 }
#endif

#if XHASH_BITS == 512
#define __v_64(                                 \
        x00, x01, x02, x03, x04, x05, x06, x07, \
        x08, x09, x10, x11, x12, x13, x14, x15, \
        x16, x17, x18, x19, x20, x21, x22, x23, \
        x24, x25, x26, x27, x28, x29, x30, x31, \
        x32, x33, x34, x35, x36, x37, x38, x39, \
        x40, x41, x42, x43, x44, x45, x46, x47, \
        x48, x49, x50, x51, x52, x53, x54, x55, \
        x56, x57, x58, x59, x60, x61, x62, x63  \
    ) {                                         \
        x00, x01, x02, x03, x04, x05, x06, x07, \
        x08, x09, x10, x11, x12, x13, x14, x15, \
        x16, x17, x18, x19, x20, x21, x22, x23, \
        x24, x25, x26, x27, x28, x29, x30, x31, \
        x32, x33, x34, x35, x36, x37, x38, x39, \
        x40, x41, x42, x43, x44, x45, x46, x47, \
        x48, x49, x50, x51, x52, x53, x54, x55, \
        x56, x57, x58, x59, x60, x61, x62, x63  \
    }
#elif XHASH_BITS == 256
#define __v_64(                                 \
        x00, x01, x02, x03, x04, x05, x06, x07, \
        x08, x09, x10, x11, x12, x13, x14, x15, \
        x16, x17, x18, x19, x20, x21, x22, x23, \
        x24, x25, x26, x27, x28, x29, x30, x31, \
        x32, x33, x34, x35, x36, x37, x38, x39, \
        x40, x41, x42, x43, x44, x45, x46, x47, \
        x48, x49, x50, x51, x52, x53, x54, x55, \
        x56, x57, x58, x59, x60, x61, x62, x63  \
    ) {                                         \
        x00, x01, x02, x03, x04, x05, x06, x07, \
        x08, x09, x10, x11, x12, x13, x14, x15, \
        x16, x17, x18, x19, x20, x21, x22, x23, \
        x24, x25, x26, x27, x28, x29, x30, x31  \
    }
#elif XHASH_BITS == 128
#define __v_64(                                 \
        x00, x01, x02, x03, x04, x05, x06, x07, \
        x08, x09, x10, x11, x12, x13, x14, x15, \
        x16, x17, x18, x19, x20, x21, x22, x23, \
        x24, x25, x26, x27, x28, x29, x30, x31, \
        x32, x33, x34, x35, x36, x37, x38, x39, \
        x40, x41, x42, x43, x44, x45, x46, x47, \
        x48, x49, x50, x51, x52, x53, x54, x55, \
        x56, x57, x58, x59, x60, x61, x62, x63  \
    ) {                                         \
        x00, x01, x02, x03, x04, x05, x06, x07, \
        x08, x09, x10, x11, x12, x13, x14, x15  \
    }
#endif

// TODO: PERMITIR INICIALIZAR OS PARAMETROS NO CREATE E NO RESET
static const xhash_s skel = {
    { // X
        __v_8 (
            0b1010100110100010111000110011111110110110000100111100011101011101ULL,
            0b1000011111001101001111010111101100100110100000100101101101100110ULL,
            0b0101010011001110100101010111001101010010010010111100111110001101ULL,
            0b0011011100110101101001110100010010001101001000110101100101001011ULL,
            0b0100110111110110010110101001011100101001010000011000101000000001ULL,
            0b0010000001101101010010111110100110110100100110100010100101111101ULL,
            0b0101111111000011011010001101110000100000010111001101100101000100ULL,
            0b1000001100100001101101111010011011011110000001001000101111000111ULL
        ), __v_8 (
            0b1111010001001011110111100001001111001110100100111100110110000000ULL,
            0b1000011110100111100010010111000000011111111000010101001010010101ULL,
            0b0001101011011111000100010000001110011001010110111011001010111010ULL,
            0b1010000110101100011101000110100111010001111101111100001011100110ULL,
            0b1011110111111110100100010110010101001100110100001010111101100000ULL,
            0b1101000011000100000111101001111100001101100100010100001100111111ULL,
            0b0101111110000001110001101100100110001100001001011101010001011000ULL,
            0b0100101011010001100100101010100011110011000101011101101110101100ULL
        ), __v_8 (
            0b0101000100101010010010100000011010101011000101111101111101001101ULL,
            0b0111010101001111000111111100110000101000000000010100101101011001ULL,
            0b1111011101010100010101101101101111101101110110110001000001001100ULL,
            0b1001100101111100010111110000101010111111011000111011000111010111ULL,
            0b0000010100010000011000010001000000000001111000011010110011010111ULL,
            0b1100001001110011100101011101111100000001000010010111110011110110ULL,
            0b1111000000111101110010111101111011011101110111010110100110101000ULL,
            0b0101010000101000100101100111110100011000001001101000011101100100ULL
        ), __v_8 (
            0b1110111000101100101010000100101011110010100010101100001110010010ULL,
            0b1111000111011101000111000110110000010011000110010010000101011101ULL,
            0b0001110000010011101110111100101110010001101001011110000101101010ULL,
            0b1000000101011011010000011011100101110011101010001000010001110101ULL,
            0b1000001010111110010010110011011110101010010010011101010101000100ULL,
            0b1100011111001001011101101001100111101010011010001010010110101001ULL,
            0b0011100011111000010011111000111110100000001111101110001011110110ULL,
            0b0001111101001110100000000111111010111101001100100101011110011001ULL
        ), __v_8 (
            0b1001111111010111101100101101110010110110101001010110010111101001ULL,
            0b0001000011011010011100000010101100001101111100111101010001001111ULL,
            0b0101010111111010001010111010000100011000100000001011100110111110ULL,
            0b1011101111001010000101110001100110001000001110101110100101100001ULL,
            0b1011000101100110111111001000100100101101011001100111010001110100ULL,
            0b1110110001110101110111111100111101101010010111001010110000010000ULL,
            0b0111111100000100100010011110101010000001100010000010000101100100ULL,
            0b0110111000001110011011101001101010000000000100001110011101111100ULL
        ), __v_8 (
            0b1010010010100011011110110001100101111000111000001001101110100101ULL,
            0b1111101001011111000110110010101100001101010011100010001110111111ULL,
            0b0111110100111111111011100000111001101001111100001010010110010001ULL,
            0b0101011000001001111011000011001110100100011110111110100111000000ULL,
            0b1010100111010101100000110110010100000101010110100001000001010010ULL,
            0b0001101001000011100000111111110101100000110100010011000011001100ULL,
            0b0101110111000110010011110000011111110001100010110010011011110100ULL,
            0b1011110111010110000001100101111000111010001111011000100010100001ULL
        ), __v_8 (
            0b0001111101101010110011000001110000110111010111000101010001111000ULL,
            0b1101110001001000110110001010111111101001111000100000110001101001ULL,
            0b0100001111011101010100110011111000100010111101111100000100111000ULL,
            0b0110111000000010000001100010011000010111010110110011111010100010ULL,
            0b1100011001000101001100010111011101000110110001010110100010011100ULL,
            0b1000011010000001101110010001101011001100111010111100100011010000ULL,
            0b0110010100111001110111111000010100001011010110010100011001110011ULL,
            0b1001111101100100111101111110001110110001110101001110101101110001ULL
        ), __v_8 (
            0b1111010101001001100101001100110011011000110010001101111011101111ULL,
            0b1001011100101110011001011111110001010010111111001000011010001011ULL,
            0b0010001111100011101011101100010011000110001001001000000000110101ULL,
            0b1001011000011011010011101011101010110001001010011000011111000010ULL,
            0b0000100101100110110101011011001101110001100100000111100011011101ULL,
            0b1101011001101001010001111010110001001110000101100000100111010110ULL,
            0b0011101111011100000101101100100010000001110010111010100011101111ULL,
            0b1111110100100001001101100010101110010100100000100101110011000111ULL
        )
    },

    __v_8 ( // A
            0b1111000000100001011100000000110010110000000100101101110111101101ULL,
            0b1101111001010101011010100011000101001110011101000101110000110101ULL,
            0b1001011111000010000000000110100111110111110011101100010110000110ULL,
            0b1011100000010111011100110100110100010110110110000101111111110010ULL,
            0b1101001010110010111110010101111111101011000010110110101110110010ULL,
            0b0000000110111001100011100011100110110110010011001110011000001010ULL,
            0b0001010111111000100111111000100111001011010000011000000111100101ULL,
            0b0010001110110011101110010101010000101001110100100111001110010010ULL
    ),

    // TEMP, TEMP SIZE
    { 0 },

    __v_64 ( // PAD
        0x0B, 0xE9, 0x10, 0x9F, 0xC5, 0xC0, 0x99, 0x1E,
        0xA7, 0xEE, 0x56, 0xBC, 0xA6, 0xA4, 0x6A, 0xB3,
        0xE5, 0x21, 0x41, 0xD7, 0x92, 0xE5, 0xA9, 0xFE,
        0xAD, 0xB0, 0xFB, 0x6C, 0xD3, 0x46, 0xC2, 0x88,
        0x17, 0x8F, 0x35, 0xD4, 0x14, 0xB0, 0xFD, 0xD9,
        0x79, 0x46, 0x7C, 0x8B, 0x22, 0x18, 0x21, 0x4D,
        0x43, 0x1C, 0x1F, 0x33, 0x75, 0xD2, 0x4F, 0xC7,
        0x2B, 0xE0, 0x2E, 0x20, 0x27, 0x8A, 0x6A, 0xDF
    )
};

// NOTE: THE HASH IS SAVED BIG ENDIAN
// TODO: restrict ctx vs ctx->temp?
// TODO: UMA VERSAO ALINHADA
static inline void __optimize xhash_do (xhash_s* const restrict ctx, const u8* restrict data, uint q) {

#define A (ctx->A)
#define X (ctx->X)

    while (q) {
           q--;

        // ORIGINAL
        xhash_t O; memcpy(&O, data, sizeof(xhash_t)); data += sizeof(xhash_t);

        //
        BUILD_ASSERT(sizeof(O) == sizeof(ctx->tmp));

        // LOCAL ENDIANESS
#if 1
        O = (xhash_t) __builtin_shuffle( (xhash_bytes_t) O,
            (xhash_bytes_t) ( (xhash_t) {
                0x0001020304050607ULL,
                0x08090A0B0C0D0E0FULL,
#if XHASH_BITS > 128
                0x1011121314151617ULL,
                0x18191A1B1C1D1E1FULL,
#if XHASH_BITS > 256
                0x2021222324252627ULL,
                0x28292A2B2C2D2E2FULL,
                0x3031323334353637ULL,
                0x38393A3B3C3D3E3FULL
#endif
#endif
            })
        );
#endif

        // ACCUMULATE AND MIX
        for (uint i = 0; i != X_LEN; i++) {

            xhash_t q = A + O;

            // YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|
            //                         + >> 32 YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
            //                                 YYYYYYYYYYYYYYYYXXXXXXXXXXXXXXXX|
            //                         + >> 16                 YYYYYYYYYYYYYYYY|XXXXXXXXXXXXXXXX
            //                                                 YYYYYYYYXXXXXXXX|
            //                         + >>  8                         YYYYYYYY|XXXXXXXX
            //                                                         YYXXXXXX|
            //                         + >>  4                             YYXX|XXXX
            //                           &                               XXXXXX|
            q += q >> 32;
            q += q >> 16;
            q += q >>  8;
            q += q >>  4;

            q &= 0b111111U; // %= 64 BITS

            // SWAP64
            // O ORIGINAL NAO PERDE NENHUM BIT POIS NAO HA OVERFLOW
            A += O = (O >> q) | (O << (64 - q));

            //
            A += (xhash_t) __builtin_shuffle(
                 (xhash_bytes_t) (X[i]),
                ((xhash_bytes_t) A) & (sizeof(xhash_bytes_t) - 1)
            );

            // OPOSITE X
            // ESCOLHE UM VETOR
            // ESCOLHE AS PALAVRAS DESTE VETOR
            // TODO: ESSE BUILTIN_SHUFFLE RETORNA MESMO ESTE TIPO?
            // [ (i, (8 - 1) - i) for i in range(8) ]
            // [(0, 7), (1, 6), (2, 5), (3, 4), (4, 3), (5, 2), (6, 1), (7, 0)]
            A += (xhash_t) __builtin_shuffle(
                 (xhash_bytes_t) (X[(X_LEN - 1) - i]),
                ((xhash_bytes_t) A) & (sizeof(xhash_bytes_t) - 1)
            );

            //
            A = X[i] += A;
        }
    }

#undef A
#undef X
}

void __optimize xhash_put (xhash_s* const restrict ctx, const u8* restrict data, uint size) {

    // NOTE: É SAFE PASSAR DATA NULL COM SIZE 0
    ASSERT(data != NULL || size == 0);

    ASSERT(CTX_TSIZE(ctx) < sizeof(ctx->tmp));

    // QUANTO TEM
    uint tsize = CTX_TSIZE(ctx);

    if (tsize) {

        ASSERT(tsize < sizeof(ctx->tmp));

        // QUANTO FALTA PARA COMPLETAR
        uint puxar = sizeof(xhash_t) - tsize;

        // SÓ PODE PEGAR O QUE TEM
        if (puxar > size)
            puxar = size;

        memcpy(ctx->tmp + tsize, data, puxar);

        // TIROU DO BUFFER...
        data += puxar;
        size -= puxar;

        // ...E COLOCOU NO TEMP
        if ((tsize += puxar) != sizeof(xhash_t)) {
            // AINDA NAO TEM UM TEMP COMPLETO
            ASSERT(size == 0);
            CTX_TSIZE(ctx) = tsize;
            return;
        }

        xhash_do(ctx, ctx->tmp, 1);
    }

    // NAO TEM TEMP
    xhash_do(ctx, data, size / sizeof(xhash_t));

    // SOBROU ISSO
    if ((tsize = size % sizeof(xhash_t)))
        memcpy(ctx->tmp, data + size - tsize, tsize);
}

void __optimize xhash_flush (xhash_s* const restrict ctx, const u8* restrict data, uint size, u8* const restrict hash, const uint hash_len) {

    ASSERT(data != NULL || size == 0);
    ASSERT(hash != NULL || hash_len == 0);

    ASSERT(hash_len <= (sizeof(xhash_t) * (X_LEN + 1)));
    
    ASSERT(CTX_TSIZE(ctx) < sizeof(ctx->tmp));

    // ????????
    if (hash_len > (sizeof(xhash_t) * (X_LEN + 1)))
        // TODO: FAILED
        return;

    //
    xhash_put(ctx, data, size);

const xhash_s backup = *ctx;

    // O PUT() CONSUMIU TODO O DATA/SIZE
    ASSERT(CTX_TSIZE(ctx) < sizeof(ctx->tmp));

    // NA VERDADE NÃO É SÓ UM PAD, VAI USAR ELE INTEIRO MESMO QUE NAO TENHA TEMP
    const uint tsize = CTX_TSIZE(ctx);

    // NOTE QUE AQUI ESTÁ OVERWRITING TSIZE...
    // ...WITH ITSELF
    memset(ctx->tmp  + tsize, tsize,
    sizeof(ctx->tmp) - tsize);

    BUILD_ASSERT((offsetof(xhash_s, tmp) + sizeof(ctx->tmp))
               == offsetof(xhash_s, pad));

    // TMP + PAD + TRAIL
    xhash_do(ctx, ctx->tmp, 2);

    //
    xhash_t result = ctx->A; // TODO: RESULT TEM QUE TER OTAMANHO >= hash_len!!!!

    // __builtin_rotateright

    // TODO: REDUCE ALL WORDS?

    // GLOBAL ENDIANESS
#if 1
    result = (xhash_t) __builtin_shuffle( (xhash_bytes_t) result,
        (xhash_bytes_t) ( (xhash_t) {
            0x0001020304050607ULL,
            0x08090A0B0C0D0E0FULL,
#if XHASH_BITS > 128
            0x1011121314151617ULL,
            0x18191A1B1C1D1E1FULL,
#if XHASH_BITS > 256
            0x2021222324252627ULL,
            0x28292A2B2C2D2E2FULL,
            0x3031323334353637ULL,
            0x38393A3B3C3D3E3FULL
#endif
#endif
        })
    );
#endif

    ASSERT(BE64(ctx->A[0]) == result[0]);
    ASSERT(BE64(ctx->A[1]) == result[1]);

    // PASS
    memcpy(hash, &result, hash_len);


*ctx = backup;
    //CTX_TSIZE(ctx) = tsize;
    //
    ASSERT(CTX_TSIZE(ctx) == tsize);
}

// REINITIALIZE
void xhash_reset (xhash_s* const ctx) {

    memcpy(ctx, &skel, sizeof(xhash_s));

    ASSERT(CTX_TSIZE(ctx) == 0);
}

xhash_s* xhash_new (void) {

    xhash_s* const ctx = aligned_alloc(sizeof(xhash_t), sizeof(xhash_s));

    if (ctx) {
        // INITIALIZE

        xhash_reset(ctx);
    }

    return ctx;
}

void xhash_free (xhash_s* const ctx) {

    ASSERT(CTX_TSIZE(ctx) < sizeof(ctx->tmp));

    free(ctx);
}

// FOR SPEED
// WE CAN ALWAYS DO A EVEN DEEPER CHECK BY VERIFYING THE HASH OF THE FILES
void __optimize xcsum (const void* restrict data, uint size, void* restrict csum);
