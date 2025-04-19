// CONFIG
#ifndef XHASH_BITS
#define XHASH_BITS 256
#define REG_ALIGN 256
#endif

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

#define BUILD_ASSERT(c) _Static_assert(c, #c)

// __attribute__((target("popcnt", "avx2")))

#if defined(__clang__)
#define __optimize // TODO:
#define __builtin_shuffle __builtin_shufflevector
#else
#define __optimize __attribute__((optimize("-O3", "-ffast-math", "-fstrict-aliasing")))
#endif

//
#define REG_BITS  XHASH_BITS // ON A REGISTER
#define WORD_BITS   64 // ON A WORD
#define CHAR_BITS    8 // ON A CHAR

#define REG_WORDS_N (REG_BITS / WORD_BITS)
#define REG_CHARS_N (REG_BITS / CHAR_BITS)

typedef u64 xword_t;
typedef u8  xchar_t;

typedef xword_t xregister_w __attribute__ ((vector_size(REG_WORDS_N * sizeof(xword_t))));
typedef xchar_t xregister_c __attribute__ ((vector_size(REG_CHARS_N * sizeof(xchar_t))));

typedef union xregister {
    xregister_w w; // QUEBRADO EM PEDACOS DE MAXIMO TAMANHO
    xregister_c c; // QUEBRADO EM PEDACOS DE MINIMO TAMANHO
} xregister;

//
BUILD_ASSERT(sizeof(xword_t)   * 8 == WORD_BITS);
BUILD_ASSERT(sizeof(xchar_t)   * 8 == CHAR_BITS);
BUILD_ASSERT(sizeof(xregister) * 8 == REG_BITS);

//
BUILD_ASSERT(sizeof(xregister_w) == sizeof(xword_t) * REG_WORDS_N);
BUILD_ASSERT(sizeof(xregister_c) == sizeof(xchar_t) * REG_CHARS_N);

//
BUILD_ASSERT(sizeof(xregister) == sizeof(xregister_w));
BUILD_ASSERT(sizeof(xregister) == sizeof(xregister_c));

#include "util.h"
#include "xmacros.h"

#define ASSERT_CTX_TSIZE(ctx) ASSERT((ctx)->tsize < sizeof((ctx)->tmp))
#define ASSERT_CTX_LOOPS(ctx) ASSERT(0 < (ctx)->loops && (ctx)->loops <= 1000)

#define REGS_SIZE (REGS_N * sizeof(xregister))

typedef struct xhash_s {
    u8          tmp [REGS_SIZE * 2]; // TMP + PAD (USEFUL TO FORCE A FINAL MIXING)
    xregister   acc [REGS_N]; // ACCUMULATOR
    xregister_c rot; // ROTATOR
    xregister_c end; // ENDIANESS
    u16 tsize;
    u16 loops;
    u32 reserved;
    u64 total;
} xhash_s;

#include "skel.c"

#define O_ROTATE_BITS(r)   orig[r].w = SWAP_LEFT(orig[r].w, WORD_BITS, r + 1)
#define O_ROTATE_CHARS(r)  orig[r].c = __builtin_shuffle(     orig[r].c, ctx->rot[r])
#define O_ENDIANESS(r)     orig[r].c = __builtin_shuffle(     orig[r].c, ctx->end )
#define A_ENDIANESS(r) ctx->acc[r].c = __builtin_shuffle( ctx->acc[r].c, ctx->end )
#define A_ADD_O(r)     ctx->acc[r].w += orig[r].w
#define A_MIX(r)       ctx->acc[r].w += (xregister_w) __builtin_shuffle ( \
                       ctx->acc[INDEX_HALFWAY(REGS_N, r)].c, \
                       ctx->acc[INDEX_OPOSITE(REGS_N, r)].c & (REG_CHARS_N - 1) \
                    )

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
        xregister orig [REGS_N]; memcpy(&orig, data, sizeof(orig)); data += sizeof(orig);

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

    xhash_s* const ctx = aligned_alloc(REG_ALIGN, sizeof(xhash_s));

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
