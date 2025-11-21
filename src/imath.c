
#include "imath.h"

#include <stddef.h>

size_t
log2sz (const size_t x)
{
    size_t y = 0;

#if defined __has_builtin && __has_builtin (__builtin_clzll)
    if (x == 0) return 0;
    y = 64 - __builtin_clzll (x);

#else
#warning "log2sz: using slow for loop, no builtin's found."
    size_t i = x;
    for (; i != 0; i >>= 1, y++) {}
#endif

    return y;
}

/* end of file */
