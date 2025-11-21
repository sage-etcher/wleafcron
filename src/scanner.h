
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

    int last_line;
    int last_column;

    int number;
    struct string string;
} scanner_t;

int scanner_init (scanner_t *self, const char *filename);
void scanner_destroy (scanner_t *self);
symbols_t scanner_scan (scanner_t *self);

#endif
/* end of file */
