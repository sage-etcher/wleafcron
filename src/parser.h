
#ifndef PARSER_H
#define PARSER_H

#include "rules.h"
#include "scanner.h"
#include "symbols.h"

typedef struct {
    scanner_t scanner;
    symbols_t syms;

    rules_t *rules;
} parser_t;

int parser_init (parser_t *self, const char *filename, rules_t *p_rules);
void parser_destroy (parser_t *self);
int parser_parse (parser_t *self);

#endif
/* end of file */
