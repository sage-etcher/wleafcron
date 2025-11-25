
#include "parser.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "rules.h"
#include "scanner.h"
#include "symbols.h"

static int sym_exists (parser_t *self, symbols_t accepted_syms);
static int accept (parser_t *self, symbols_t accepted_syms);
static void expect (parser_t *self, symbols_t expected_syms);
static int stmt (parser_t *self);
static void stmtseq (parser_t *self);

static int
sym_exists (parser_t *self, symbols_t accepted_syms)
{
    size_t ii = 0;
    size_t jj = 0;
    for (; ii < self->syms.cnt; ii++)
    {
        for (jj = 0; jj < accepted_syms.cnt; jj++)
        {
            if (self->syms.m[ii] == accepted_syms.m[jj])
            {
                return 1;
            }
        }
    }

    return 0;
}

static int
accept (parser_t *self, symbols_t accepted_syms)
{
    int rc = sym_exists (self, accepted_syms);
    if (rc)
    {
        self->syms = scanner_scan (&self->scanner);
    }

    return rc;
}

static void
expect (parser_t *self, symbols_t expected_syms)
{
    if (!accept (self, expected_syms))
    {
        fprintf (stderr, "error: %d %d: syntax error\n",
                self->scanner.last_line, self->scanner.last_column);
        exit (EXIT_FAILURE);
    }
}

int
parser_init (parser_t *self, const char *filename, rules_t *p_rules)
{
    self->syms = (symbols_t){ .cnt = 0, .m = { 0 } };
    self->rules = p_rules;

    return scanner_init (&self->scanner, filename);
}

void
parser_destroy (parser_t *self)
{
    self->syms = (symbols_t){ .cnt = 0, .m = { 0 } };

    scanner_destroy (&self->scanner);
}

// rule = [ param param param param param command [arg...] ] .
static int
parse_rule (parser_t *self)
{
    size_t i = 0;
    singlerule_t data = { 0 };

    singlerule_init (&data);

    /* get time parameters from line */
    for (; i < RULE_PARAM_COUNT; i++)
    {
        if (!sym_exists (self, SYMBOLS2 (SYM_GENERIC, SYM_NUMBER)))
        {
            fprintf (stderr, "error: %d %d: expected a proper date\n",
                    self->scanner.last_line, self->scanner.last_column);
            return 1;
        }

        /* throw error when neither (param == '*') nor (min <= param <= max) */
        if (!((RULE_GENERIC == self->scanner.number) ||
              ((RULE_PARAM_MIN[i] <= self->scanner.number) &&
               (RULE_PARAM_MAX[i] >= self->scanner.number))))
        {
            fprintf (stderr, "error: %d %d: %s can only be * or value %d-%d\n",
                    self->scanner.last_line, self->scanner.last_column,
                    RULE_PARAM_NAME[i], RULE_PARAM_MIN[i], RULE_PARAM_MAX[i]);
            return 1;
        }

        data.time[i] = self->scanner.number;
        self->syms = scanner_scan (&self->scanner);
    }

    /* get command and args */
    while (sym_exists (self, SYMBOLS1 (SYM_STRING)))
    {
        singlerule_add_arg (&data, strdup (self->scanner.string.m));
        self->syms = scanner_scan (&self->scanner);
    }

    if (data.count == 0)
    {
        fprintf (stderr, "error: %d %d: expected a command\n",
                self->scanner.last_line, self->scanner.last_column);
        return 1;
    }

    /* append the data to parser.rules */
    rules_add (self->rules, data);

    return 0;
}

// stmt = [ newline | endoffile | comment | rule ] .
static int
stmt (parser_t *self)
{

    if (sym_exists (self, SYMBOLS2 (SYM_NEWLINE, SYM_EOF))) return 0;

    if (accept (self, SYMBOLS1 (SYM_COMMENT))) return 0;

    return parse_rule (self);
}

static void
stmtseq (parser_t *self)
{
    int rc = 0;
    do {
        rc = stmt (self);
    } while (!rc && accept (self, SYMBOLS1 (SYM_NEWLINE)));

}

int
parser_parse (parser_t *self)
{
    self->syms = scanner_scan (&self->scanner);
    //(void)accept (self, SYMBOLS1 (SYM_NEWLINE));

    stmtseq (self);
    expect (self, SYMBOLS1 (SYM_EOF));

    return 0;
}

/* end of file */
