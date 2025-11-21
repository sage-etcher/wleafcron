
#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

#include "morestrings.h"
#include "symbols.h"

typedef struct {
    const char *filename;
    FILE *fp;
    char ch;

    int line;
    int column;

    int number;
    struct string command;
} scanner_t;

typedef struct {
    enum { MAX_POTENTIAL_SYMBOLS = 2 };
    symbol_t m[MAX_POTENTIAL_SYMBOLS];
    size_t cnt;
} symbols_t;

int scanner_init (scanner_t *self, const char *filename);
void scanner_destroy (scanner_t *self);
symbols_t scanner_scan (scanner_t *self);

#endif
/* end of file */
