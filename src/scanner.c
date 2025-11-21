
#include "scanner.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "morestrings.h"
#include "symbols.h"

#define SYMBOLS1(x)    (symbols_t){ .cnt = 1, .m = { x }    }
#define SYMBOLS2(x, y) (symbols_t){ .cnt = 2, .m = { x, y } }

static void
readch (scanner_t *self)
{
    self->ch = getc (self->fp);

    if (self->ch == '\n')
    {
        self->line++;
        self->column = 1;
    }
    else if (self->ch != '\r')
    {
        self->column++;
    }
}

int
scanner_init (scanner_t *self, const char *filename)
{
    self->fp = fopen (filename, "r");
    if (self->fp == NULL)
    {
        return errno;
    }

    self->filename = filename;
    self->ch = '\0';

    self->line = 1;
    self->column = 1;

    self->number = 0;
    string_init (&self->command, 0);

    readch (self);

    return 0;
}

void
scanner_destroy (scanner_t *self)
{
    if (self == NULL) return;

    fclose (self->fp);

    self->filename = NULL;
    self->fp = NULL;
    self->ch = '\0';

    self->line = 0;
    self->column = 0;

    self->number = 0;

    free (self->command.m);
    self->command.m = NULL;
    self->command.alloc = 0;
    self->command.count = 0;
}


symbols_t
scan_string (scanner_t *self)
{
    size_t i = 0;

    self->command.count = 0;
    for (; !isspace (self->ch) && !feof (self->fp); i++, readch (self))
    {
        string_extend (&self->command, i);
        self->command.m[i] = self->ch;
    }
    self->command.m[i] = '\0';
    self->command.count = i;

    return SYMBOLS1 (SYM_STRING);
}

symbols_t
scan_maybe_string_number (scanner_t *self)
{
    size_t i = 0;
    int val = 0;

    (void)scan_string (self);
    for (; i < self->command.count; i++)
    {
        /* cannot be a number */
        if (!isdigit (self->command.m[i]))
        {
            return SYMBOLS1 (SYM_STRING);
        }

        /* process digitis, generate val */
        val = self->command.m[i] - '0' + val * 10;
    }

    self->number = val;

    return SYMBOLS2 (SYM_NUMBER, SYM_STRING);
}

symbols_t
scan_quoted_string (scanner_t *self)
{
    size_t i = 0;
    char quote = self->ch;

    readch (self);  /* pop the initial quote */

    self->command.count = 0;
    for (; self->ch != quote; i++, readch (self))
    {
        if (feof (self->fp) || self->ch == '\n' || self->ch == '\r')
        {
            return SYMBOLS1 (SYM_UNKNOWN);
        }

        string_extend (&self->command, i);
        self->command.m[i] = self->ch;
    }
    self->command.m[i] = '\0';
    self->command.count = i;

    readch (self);  /* pop the trailing quote */

    return SYMBOLS1 (SYM_STRING);
}

symbols_t
scan_comment (scanner_t *self)
{
    size_t i = 0;

    readch (self);  /* pop # */

    self->command.count = 0;
    for (;self->ch != '\n' && !feof (self->fp); i++, readch (self))
    {
        string_extend (&self->command, i);
        self->command.m[i] = self->ch;
    }
    self->command.m[i] = '\0';
    self->command.count = i;

    return SYMBOLS1 (SYM_COMMENT);
}

symbols_t 
scan_maybe_string_generic (scanner_t *self)
{
    (void)scan_string (self);

    if (self->command.count == 1)
    {
        return SYMBOLS2 (SYM_GENERIC, SYM_STRING);
    }

    return SYMBOLS1 (SYM_STRING);
}

symbols_t
scanner_scan (scanner_t *self)
{
    /* skip whitespace */
    while (isblank (self->ch) && !feof (self->fp))
    {
        readch (self);
    }

    if (feof (self->fp)) return SYMBOLS1 (SYM_EOF);

    switch (self->ch)
    {
    case '\n': readch (self); return SYMBOLS1 (SYM_NEWLINE);

    case '#': return scan_comment (self);
    case '*': return scan_maybe_string_generic (self);

    case '\'': case '"': return scan_quoted_string (self);
    }

    return scan_maybe_string_number (self);
}


/* end of file */
