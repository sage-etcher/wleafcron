
#include "symbols.h"

const char *SYM_TEXT[SYM_COUNT] = {
    [SYM_UNKNOWN] = "SYM_UNKNOWN",
    [SYM_COMMENT] = "SYM_COMMENT",
    [SYM_GENERIC] = "SYM_GENERIC",
    [SYM_NUMBER]  = "SYM_NUMBER",
    [SYM_STRING]  = "SYM_STRING",
    [SYM_NEWLINE] = "SYM_NEWLINE",
    [SYM_EOF]     = "SYM_EOF",
};


int
sym_exists (symbols_t self, symbols_t accepted_syms)
{
    size_t ii = 0;
    size_t jj = 0;
    for (; ii < self.cnt; ii++)
    {
        for (jj = 0; jj < accepted_syms.cnt; jj++)
        {
            if (self.m[ii] == accepted_syms.m[jj])
            {
                return 1;
            }
        }
    }

    return 0;
}

/* end of file */
