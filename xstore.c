
#define  XHASH_BITS     512
#define  words_v        words512_v
#define  chars_v        chars512_v
#define  verse_v        verse512_v
#define  xhash_s        xhash512_s
#define  xhash_do       xhash512_do
#define  xhash_put      xhash512_put
#define  xhash_flush    xhash512_flush
#define  xhash_new      xhash512_new
#define  xhash_reset    xhash512_reset
#define  xhash_free     xhash512_free
#define  skel           skel512
#include "xhash.c"
#undef   XHASH_BITS
#undef   words_v
#undef   chars_v
#undef   verse_v
#undef   xhash_s
#undef   xhash_free
#undef   xhash_do
#undef   xhash_put
#undef   xhash_flush
#undef   xhash_reset
#undef   xhash_new
#undef   skel
#undef   __chars
#undef   __words

#define  XHASH_BITS     256
#define  words_v        words256_v
#define  chars_v        chars256_v
#define  verse_v        verse256_v
#define  xhash_s        xhash256_s
#define  xhash_do       xhash256_do
#define  xhash_put      xhash256_put
#define  xhash_flush    xhash256_flush
#define  xhash_new      xhash256_new
#define  xhash_reset    xhash256_reset
#define  xhash_free     xhash256_free
#define  skel           skel256
#include "xhash.c"
#undef   XHASH_BITS
#undef   words_v
#undef   chars_v
#undef   verse_v
#undef   xhash_s
#undef   xhash_free
#undef   xhash_do
#undef   xhash_put
#undef   xhash_flush
#undef   xhash_reset
#undef   xhash_new
#undef   skel
#undef   __chars
#undef   __words

#define  XHASH_BITS     128
#define  words_v        words128_v
#define  chars_v        chars128_v
#define  verse_v        verse128_v
#define  xhash_s        xhash128_s
#define  xhash_do       xhash128_do
#define  xhash_put      xhash128_put
#define  xhash_flush    xhash128_flush
#define  xhash_new      xhash128_new
#define  xhash_reset    xhash128_reset
#define  xhash_free     xhash128_free
#define  skel           skel128
#include "xhash.c"
#undef   XHASH_BITS
#undef   words_v
#undef   chars_v
#undef   verse_v
#undef   xhash_s
#undef   xhash_free
#undef   xhash_do
#undef   xhash_put
#undef   xhash_flush
#undef   xhash_reset
#undef   xhash_new
#undef   skel
#undef   __chars
#undef   __words
