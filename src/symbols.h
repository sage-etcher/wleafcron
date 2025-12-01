
#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stddef.h>

typedef enum {
    SYM_UNKNOWN = 0,
    SYM_COMMENT,
    SYM_GENERIC,
    SYM_NUMBER,
    SYM_STRING,
    SYM_NEWLINE,
    SYM_EOF,
    SYM_COUNT,
} symbol_t;

extern const char *SYM_TEXT[SYM_COUNT];

#define MAX_POTENTIAL_SYMBOLS 2

typedef struct {
    symbol_t m[MAX_POTENTIAL_SYMBOLS];
    size_t cnt;
} symbols_t;

#define SYMBOLS1(x)    (symbols_t){ .cnt = 1, .m = { x }    }
#define SYMBOLS2(x, y) (symbols_t){ .cnt = 2, .m = { x, y } }

int sym_exists (symbols_t self, symbols_t accepted_syms);

#endif
/* end of file */
