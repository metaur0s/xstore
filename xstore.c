
#ifndef XHASH_BITS

#define XHASH_BITS  512
#define REG_ALIGN   256
#define REGS_N      32
#define xregister   xregister512
#define xregister_w xregister512_w
#define xregister_c xregister512_c
#define xhash_s     xhash512_s
#define xhash_do    xhash512_do
#define xhash_put   xhash512_put
#define xhash_flush xhash512_flush
#define xhash_new   xhash512_new
#define xhash_reset xhash512_reset
#define xhash_free  xhash512_free
#define skel        skel512

#include "xstore.c"

#else //

#include "xhash.c"

#undef   XHASH_BITS
#undef   xregister      
#undef   xregister_w    
#undef   xregister_c    
#undef   xhash_s
#undef   xhash_free
#undef   xhash_do
#undef   xhash_put
#undef   xhash_flush
#undef   xhash_reset
#undef   xhash_new
#undef   skel
#undef   REG_W
#undef   REG_C
#undef   REGS
#undef   REGS_N
#undef   REG_ALIGN
#undef   R_INLINE

#endif
