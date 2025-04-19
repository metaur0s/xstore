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

#define REGS_SIZE (REGISTERS_N * sizeof(xregister))

typedef struct xhash_s {
    u8          tmp [REGS_SIZE * 2]; // TMP + PAD (USEFUL TO FORCE A FINAL MIXING)
    xregister   acc [REGISTERS_N]; // ACCUMULATOR
    xregister_c rot; // ROTATOR
    xregister_c end; // ENDIANESS
    u16 tsize;
    u16 loops;
    u32 reserved;
    u64 total;
} xhash_s;

static const xhash_s skel = {

    .tmp = {

        // TEMP
        // any(print(', '.join([ '0x%02X' % 0  for _ in range(4 * 8)]) + ',') for _ in range(16) )
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

        // PAD
        // any(print(', '.join([ '0x%02X' % random.randint(0, 255)  for _ in range(4 * 8)]) + ',') for _ in range(16) )
        0xC2, 0xD6, 0x32, 0x02, 0xC5, 0x3C, 0x31, 0x55, 0xAD, 0xD8, 0x30, 0xEC, 0x7E, 0xBB, 0xB0, 0xF2, 0x93, 0xB4, 0x40, 0x19, 0x3B, 0xA9, 0x87, 0xDF, 0x27, 0xA8, 0xDA, 0xAA, 0x26, 0xCB, 0x1B, 0xFD,
        0x78, 0xEE, 0x1C, 0x14, 0x09, 0x0D, 0xF0, 0xC5, 0xEE, 0xAA, 0xD5, 0xA0, 0xA8, 0xAE, 0x14, 0xEA, 0x35, 0xF5, 0xD2, 0x20, 0x27, 0x00, 0xF2, 0xC8, 0x14, 0xC4, 0x5D, 0x5D, 0x0F, 0x71, 0x34, 0x70,
        0x02, 0x36, 0x5E, 0xAD, 0x78, 0x3B, 0x20, 0xD2, 0xA8, 0x53, 0xB8, 0xDE, 0xF4, 0xF5, 0x1C, 0xC1, 0xE5, 0xBB, 0x26, 0xDD, 0x87, 0x86, 0x77, 0x1F, 0x31, 0x41, 0x5D, 0x71, 0x33, 0x80, 0x8C, 0xF4,
        0x89, 0x48, 0x98, 0xE4, 0x2B, 0x2F, 0xCB, 0xB8, 0x82, 0x60, 0x91, 0x7F, 0x12, 0x53, 0xCB, 0xD2, 0xF9, 0xC6, 0x83, 0x60, 0x3E, 0xBD, 0x82, 0x38, 0xE0, 0x93, 0xDF, 0x9F, 0xA0, 0xBA, 0x13, 0x1E,
        0x9D, 0xF0, 0x7A, 0xE4, 0x37, 0x78, 0x25, 0xA6, 0x34, 0xAF, 0x7A, 0x70, 0x89, 0x59, 0x49, 0xD5, 0x4D, 0x4E, 0xDF, 0x02, 0x53, 0x4A, 0xF1, 0x71, 0xC7, 0x1E, 0xB9, 0xB2, 0x46, 0xE2, 0x86, 0x3D,
        0x9B, 0xE7, 0xAC, 0x8D, 0x6A, 0x4E, 0xD2, 0x56, 0x90, 0x34, 0x82, 0x3A, 0x81, 0x1C, 0xE4, 0xC3, 0x8D, 0xF7, 0xB1, 0x88, 0xA5, 0x2D, 0x21, 0xE0, 0x0F, 0x9A, 0xA9, 0xD3, 0x40, 0x81, 0x28, 0x18,
        0xE4, 0xCE, 0x73, 0x17, 0x34, 0x45, 0x89, 0x77, 0x32, 0xBD, 0x44, 0xA4, 0x8F, 0xA4, 0x3A, 0xBB, 0x2A, 0x6D, 0x31, 0x93, 0x1D, 0xEA, 0x00, 0x72, 0x91, 0xD6, 0xE9, 0xE2, 0x3D, 0x4E, 0x83, 0xE8,
        0x84, 0xF6, 0x0C, 0x97, 0x07, 0x39, 0x88, 0x49, 0x38, 0x7A, 0x34, 0x3B, 0xBC, 0xFD, 0xA2, 0xA6, 0x59, 0xF2, 0x62, 0x72, 0x61, 0xC6, 0x94, 0x37, 0x58, 0x43, 0x24, 0x12, 0x39, 0xA3, 0xA7, 0xD6,
        0x05, 0x4C, 0x95, 0x20, 0x9D, 0xEA, 0x4E, 0x5B, 0xE5, 0xEF, 0xB0, 0x1C, 0x00, 0x19, 0xCD, 0x60, 0x86, 0x69, 0x23, 0xE4, 0x7D, 0x23, 0xB1, 0x92, 0x44, 0x3F, 0xBE, 0x1B, 0xFC, 0xA2, 0xED, 0xCA,
        0x4F, 0xFE, 0x8E, 0xFA, 0xFA, 0x08, 0x08, 0x57, 0xC8, 0xF4, 0xBD, 0x0A, 0x80, 0xED, 0x32, 0x0E, 0xBB, 0x1C, 0xF2, 0x73, 0x99, 0x9C, 0x19, 0xAB, 0x4B, 0x2D, 0x02, 0xD7, 0x25, 0x31, 0xF7, 0x1A,
        0x88, 0xA5, 0x3F, 0x2D, 0xD3, 0xAF, 0x5D, 0x26, 0x22, 0xC2, 0x42, 0x99, 0xFF, 0x9F, 0xA6, 0x94, 0xB8, 0x65, 0x44, 0x1B, 0xB3, 0x18, 0x87, 0x3C, 0xAB, 0x7D, 0xC4, 0xC6, 0x41, 0x38, 0x7E, 0x9A,
        0xAF, 0x83, 0x00, 0x98, 0x81, 0xD0, 0x16, 0x5F, 0x05, 0xC2, 0x3E, 0x5B, 0x78, 0xF8, 0x67, 0x38, 0x29, 0xF0, 0x81, 0x03, 0xE8, 0x46, 0x63, 0x0C, 0x27, 0x41, 0x07, 0xCE, 0xA2, 0xC1, 0xAE, 0xED,
        0x4F, 0x94, 0x32, 0x4F, 0x48, 0xE1, 0x38, 0x83, 0x3C, 0xDE, 0x98, 0x3E, 0x96, 0xAD, 0xB7, 0xF6, 0x67, 0x68, 0x41, 0x37, 0x8F, 0xDF, 0x31, 0xD2, 0xC1, 0x58, 0xD9, 0x9C, 0x13, 0x2F, 0xA9, 0x57,
        0x92, 0x83, 0x78, 0xAA, 0xE4, 0x57, 0x46, 0xC4, 0xB8, 0x76, 0x83, 0x21, 0x90, 0x35, 0x18, 0x0F, 0x05, 0xB7, 0x1A, 0x22, 0xE4, 0xDE, 0xF6, 0xD4, 0x69, 0x29, 0xFA, 0x01, 0x82, 0x75, 0xD4, 0x7E,
        0x19, 0x48, 0x55, 0x4C, 0x2B, 0x15, 0x29, 0xFF, 0xA3, 0xAA, 0x94, 0x2B, 0x85, 0x94, 0x3C, 0xF9, 0xAA, 0x34, 0xB8, 0x64, 0x49, 0x7F, 0x73, 0xBF, 0xDA, 0x5C, 0xD5, 0x23, 0x30, 0xD2, 0x8A, 0xD1,
        0xFC, 0xC5, 0x32, 0x52, 0x98, 0x77, 0xEA, 0x99, 0x9C, 0x54, 0x11, 0x49, 0xB2, 0x3C, 0xE5, 0xD9, 0x72, 0xCF, 0x3F, 0x90, 0x61, 0x01, 0x30, 0x18, 0xEE, 0xE2, 0xBA, 0xC3, 0xE6, 0x2A, 0x3B, 0x18,
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
        uint puxar = REGS_SIZE - tsize;

        // SÓ PODE PEGAR O QUE TEM
        if (puxar > size)
            puxar = size;

        memcpy(ctx->tmp + tsize, data, puxar);

        // TIROU DO BUFFER...
        data += puxar;
        size -= puxar;

        // ...E COLOCOU NO TEMP
        if ((tsize += puxar) != REGS_SIZE) {
            // AINDA NAO TEM UM TEMP COMPLETO
            ASSERT(size == 0);
            ctx->tsize = tsize;
            return;
        }

        xhash_do(ctx, ctx->tmp, 1);
    }

    // NAO TEM TEMP
    xhash_do(ctx, data, size / REGS_SIZE);

    // SOBROU ISSO
    if ((tsize = size % REGS_SIZE))
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

        // PAD/FINALIZER
        memcpy(ctx->tmp + tsize,
               ctx->tmp + tsize + REGS_SIZE,
              REGS_SIZE - tsize);

        xhash_do(ctx, ctx->tmp, 2);

        // GLOBAL ENDIANESS
        DO_A_ENDIANESS;

        // PASS
        memcpy(hash, &ctx->acc, hash_len);

        //
        *ctx = backup;
    }
}

// REINITIALIZE
void xhash_reset (xhash_s* const ctx) {

    memcpy(ctx, &skel, sizeof(skel));

    ASSERT(ctx->tsize == 0);

    ASSERT_CTX_TSIZE(ctx);
    ASSERT_CTX_LOOPS(ctx);
}

xhash_s* xhash_new (void) {

    xhash_s* const ctx = aligned_alloc(XALIGNMENT, sizeof(xhash_s));

    if (ctx) {
        // INITIALIZE

        xhash_reset(ctx);
    }

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
