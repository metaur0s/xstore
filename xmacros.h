
#if REGS_N == 32
#define REGS(r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31) \
             r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31
#elif REGS_N == 16
#define REGS(r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31) \
             r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15
#elif REGS_N == 8
#define REGS(r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31) \
             r0, r1, r2, r3, r4, r5, r6, r7
#endif

// TODO: QUAL A ORDEM DESSAS WORDS?
#if REG_WORDS_N == 8
#define REG_W(w7, w6, w5, w4, w3, w2, w1, w0) \
              w7, w6, w5, w4, w3, w2, w1, w0
#elif REG_WORDS_N == 4
#define REG_W(w7, w6, w5, w4, w3, w2, w1, w0) \
                              w3, w2, w1, w0
#elif REG_WORDS_N == 2
#define REG_W(w7, w6, w5, w4, w3, w2, w1, w0) \
                                      w1, w0
#endif

// TODO: OS CARACTERES COMECAM DA ESQUUERDA PARA A DIREITA
#if REG_CHARS_N == 64
#define REG_C(                                  \
        c00, c01, c02, c03, c04, c05, c06, c07, \
        c08, c09, c10, c11, c12, c13, c14, c15, \
        c16, c17, c18, c19, c20, c21, c22, c23, \
        c24, c25, c26, c27, c28, c29, c30, c31, \
        c32, c33, c34, c35, c36, c37, c38, c39, \
        c40, c41, c42, c43, c44, c45, c46, c47, \
        c48, c49, c50, c51, c52, c53, c54, c55, \
        c56, c57, c58, c59, c60, c61, c62, c63  \
    )                                           \
        c00, c01, c02, c03, c04, c05, c06, c07, \
        c08, c09, c10, c11, c12, c13, c14, c15, \
        c16, c17, c18, c19, c20, c21, c22, c23, \
        c24, c25, c26, c27, c28, c29, c30, c31, \
        c32, c33, c34, c35, c36, c37, c38, c39, \
        c40, c41, c42, c43, c44, c45, c46, c47, \
        c48, c49, c50, c51, c52, c53, c54, c55, \
        c56, c57, c58, c59, c60, c61, c62, c63
#elif REG_CHARS_N == 32
#define REG_C(                                  \
        c00, c01, c02, c03, c04, c05, c06, c07, \
        c08, c09, c10, c11, c12, c13, c14, c15, \
        c16, c17, c18, c19, c20, c21, c22, c23, \
        c24, c25, c26, c27, c28, c29, c30, c31, \
        c32, c33, c34, c35, c36, c37, c38, c39, \
        c40, c41, c42, c43, c44, c45, c46, c47, \
        c48, c49, c50, c51, c52, c53, c54, c55, \
        c56, c57, c58, c59, c60, c61, c62, c63  \
    )                                           \
        c00, c01, c02, c03, c04, c05, c06, c07, \
        c08, c09, c10, c11, c12, c13, c14, c15, \
        c16, c17, c18, c19, c20, c21, c22, c23, \
        c24, c25, c26, c27, c28, c29, c30, c31
#elif REG_CHARS_N == 16
#define REG_C(                                  \
        c00, c01, c02, c03, c04, c05, c06, c07, \
        c08, c09, c10, c11, c12, c13, c14, c15, \
        c16, c17, c18, c19, c20, c21, c22, c23, \
        c24, c25, c26, c27, c28, c29, c30, c31, \
        c32, c33, c34, c35, c36, c37, c38, c39, \
        c40, c41, c42, c43, c44, c45, c46, c47, \
        c48, c49, c50, c51, c52, c53, c54, c55, \
        c56, c57, c58, c59, c60, c61, c62, c63  \
    )                                           \
        c00, c01, c02, c03, c04, c05, c06, c07, \
        c08, c09, c10, c11, c12, c13, c14, c15
#endif

//
#define R_LOOP(OP) { for (uint r = 0; r != REGS_N; r++) { OP(r); }}

#if REGS_N == 32
#define R_INLINE(OP) { \
        OP( 0); OP( 1); OP( 2); OP( 3); OP( 4); OP( 5); OP( 6); OP( 7); \
        OP( 8); OP( 9); OP(10); OP(11); OP(12); OP(13); OP(14); OP(15); \
        OP(16); OP(17); OP(18); OP(19); OP(20); OP(21); OP(22); OP(23); \
        OP(24); OP(25); OP(26); OP(27); OP(28); OP(29); OP(30); OP(31); }
#elif REGS_N == 16
#define R_INLINE(OP) { \
        OP( 0); OP( 1); OP( 2); OP( 3); OP( 4); OP( 5); OP( 6); OP( 7); \
        OP( 8); OP( 9); OP(10); OP(11); OP(12); OP(13); OP(14); OP(15); }
#elif REGS_N == 8
#define R_INLINE(OP) { \
        OP( 0); OP( 1); OP( 2); OP( 3); OP( 4); OP( 5); OP( 6); OP( 7); }
#endif
