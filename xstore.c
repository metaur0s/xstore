
#define  XHASH_BITS    512
#define  xhash_t       xhash512_t
#define  xhash_bytes_t xhash512_bytes_t
#define  xhash_s       xhash512_s
#define  xhash_do      xhash512_do
#define  xhash_iter    xhash512_iter
#define  xhash_done    xhash512_done
#define  xhash_new     xhash512_new
#define  xhash_reset   xhash512_reset
#define  xhash_free    xhash512_free
#define  skel          skel512
#include "skel.c"
#undef   XHASH_BITS
#undef   xhash_t
#undef   xhash_bytes_t
#undef   xhash_s
#undef   xhash_do
#undef   xhash_iter
#undef   xhash_done
#undef   xhash_new
#undef   xhash_reset
#undef   xhash_free
#undef   skel
#undef __v_64
#undef __v_8

#define  XHASH_BITS    256
#define  xhash_t       xhash256_t
#define  xhash_bytes_t xhash256_bytes_t
#define  xhash_s       xhash256_s
#define  xhash_do      xhash256_do
#define  xhash_iter    xhash256_iter
#define  xhash_done    xhash256_done
#define  xhash_new     xhash256_new
#define  xhash_reset   xhash256_reset
#define  xhash_free    xhash256_free
#define  skel          skel256
#include "skel.c"
#undef   XHASH_BITS
#undef   xhash_t
#undef   xhash_bytes_t
#undef   xhash_s
#undef   xhash_do
#undef   xhash_iter
#undef   xhash_done
#undef   xhash_new
#undef   xhash_free
#undef   xhash_reset
#undef   skel
#undef __v_64
#undef __v_8

#define  XHASH_BITS    128
#define  xhash_t       xhash128_t
#define  xhash_bytes_t xhash128_bytes_t
#define  xhash_s       xhash128_s
#define  xhash_do      xhash128_do
#define  xhash_iter    xhash128_iter
#define  xhash_done    xhash128_done
#define  xhash_new     xhash128_new
#define  xhash_reset   xhash128_reset
#define  xhash_free    xhash128_free
#define  skel          skel128
#include "skel.c"
#undef   XHASH_BITS
#undef   xhash_t
#undef   xhash_bytes_t
#undef   xhash_s
#undef   xhash_do
#undef   xhash_iter
#undef   xhash_done
#undef   xhash_new
#undef   xhash_reset
#undef   xhash_free
#undef   skel
#undef __v_64
#undef __v_8
