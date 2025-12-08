
#include "scanner.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "morestrings.h"
#include "rules.h"
#include "symbols.h"


static void
readch (scanner_t *self)
{
    if (self == NULL)
    {
        errno = EINVAL;
        return;
    }

    self->ch = (char)(unsigned)getc (self->fp);

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
    int rc = 0;

    if ((self == NULL) || (filename == NULL))
    {
        return (errno = EINVAL);
    }

    self->fp = fopen (filename, "r");
    if (self->fp == NULL)
    {
        LOG_E ("scanner cannot open file for reading");
        return errno;
    }

    self->filename = filename;
    self->ch = '\0';

    self->line   = 1;
    self->column = 1;

    self->last_line   = 1;
    self->last_column = 1;

    self->number = 0;

    rc = string_init (&self->string, 0);
    if (rc)
    {
        LOG_E ("failed to initialize scanner_t->string");
        return rc;
    }

    readch (self);

    return 0;
}

void
scanner_destroy (scanner_t *self)
{
    if (self == NULL) 
    {
        return;
    }

    (void)fclose (self->fp);

    self->filename = NULL;
    self->fp = NULL;
    self->ch = '\0';

    self->line   = 0;
    self->column = 0;

    self->last_line = 0;
    self->column = 0;

    self->number = 0;

    free (self->string.m);
    self->string.m = NULL;
    self->string.alloc = 0;
    self->string.count = 0;
}


symbols_t
scan_string (scanner_t *self)
{
    int rc = 0;
    size_t i = 0;

    if (self == NULL)
    {
        errno = EINVAL;
        return SYMBOLS1 (SYM_UNKNOWN);
    }

    self->string.count = 0;
    for (; !isspace (self->ch) && !feof (self->fp); i++, readch (self))
    {
        rc = string_extend (&self->string, i+1);
        if (rc)
        {
            LOG_E ("failed to extend string, cannot add next character");
            return SYMBOLS1 (SYM_UNKNOWN);
        }

        self->string.m[i] = self->ch;
    }
    self->string.m[i] = '\0';
    self->string.count = i;

    return SYMBOLS1 (SYM_STRING);
}

symbols_t
scan_maybe_string_number (scanner_t *self)
{
    size_t i = 0;
    int val = 0;
    symbols_t syms = { 0 };
    const int BASE = 10;

    if (self == NULL)
    {
        errno = EINVAL;
        return SYMBOLS1 (SYM_UNKNOWN);
    }

    syms = scan_string (self);
    if (sym_exists (syms, SYMBOLS1 (SYM_UNKNOWN)))
    {
        return syms;
    }

    for (; i < self->string.count; i++)
    {
        /* cannot be a number */
        if (!isdigit (self->string.m[i]))
        {
            return SYMBOLS1 (SYM_STRING);
        }

        /* process digitis, generate val */
        val = self->string.m[i] - '0' + val * BASE;
    }

    self->number = val;

    return SYMBOLS2 (SYM_NUMBER, SYM_STRING);
}

symbols_t
scan_quoted_string (scanner_t *self)
{
    int rc = 0;
    size_t i = 0;
    char quote = self->ch;

    if (self == NULL)
    {
        errno = EINVAL;
        return SYMBOLS1 (SYM_UNKNOWN);
    }

    readch (self);  /* pop the initial quote */

    self->string.count = 0;
    for (; self->ch != quote; i++, readch (self))
    {
        if (feof (self->fp) || self->ch == '\n' || self->ch == '\r')
        {
            return SYMBOLS1 (SYM_UNKNOWN);
        }

        rc = string_extend (&self->string, i+1);
        if (rc)
        {
            LOG_E ("failed to extend string, cannot add next character");
            return SYMBOLS1 (SYM_UNKNOWN);
        }

        self->string.m[i] = self->ch;
    }
    self->string.m[i] = '\0';
    self->string.count = i;

    readch (self);  /* pop the trailing quote */

    return SYMBOLS1 (SYM_STRING);
}

symbols_t
scan_comment (scanner_t *self)
{
    int rc = 0;
    size_t i = 0;

    if (self == NULL)
    {
        errno = EINVAL;
        return SYMBOLS1 (SYM_UNKNOWN);
    }

    readch (self);  /* pop # */

    self->string.count = 0;
    for (; self->ch != '\n' && !feof (self->fp); i++, readch (self))
    {
        rc = string_extend (&self->string, i+1);
        if (rc)
        {
            LOG_E ("failed to extend string, cannot add next character");
            return SYMBOLS1 (SYM_UNKNOWN);
        }

        self->string.m[i] = self->ch;
    }
    self->string.m[i] = '\0';
    self->string.count = i;

    return SYMBOLS1 (SYM_COMMENT);
}

symbols_t 
scan_maybe_string_generic (scanner_t *self)
{
    symbols_t syms = { 0 };

    if (self == NULL)
    {
        errno = EINVAL;
        return SYMBOLS1 (SYM_UNKNOWN);
    }

    syms = scan_string (self);
    if (sym_exists (syms, SYMBOLS1 (SYM_UNKNOWN)))
    {
        return syms;
    }

    if (self->string.count == 1)
    {
        self->number = RULE_GENERIC;
        return SYMBOLS2 (SYM_GENERIC, SYM_STRING);
    }

    return SYMBOLS1 (SYM_STRING);
}

symbols_t
scanner_scan (scanner_t *self)
{
    if (self == NULL)
    {
        errno = EINVAL;
        return SYMBOLS1 (SYM_UNKNOWN);
    }

    /* skip whitespace */
    while (isblank (self->ch) && !feof (self->fp))
    {
        readch (self);
    }

    self->last_line   = self->line;
    self->last_column = self->column - 1;

    if (feof (self->fp)) 
    {
        return SYMBOLS1 (SYM_EOF);
    }

    switch (self->ch)
    {
    case '\n': 
        readch (self); 
        return SYMBOLS1 (SYM_NEWLINE);

    case '#': return scan_comment (self);
    case '*': return scan_maybe_string_generic (self);

    case '\'': case '"':
        return scan_quoted_string (self);

    default:
        return scan_maybe_string_number (self);
    }
}

/* end of file */
