
#ifndef SYMBOLS_H
#define SYMBOLS_H

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

#endif
/* end of file */
