
#include <ctype.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "commonos.h"
#include "config.h"
#include "morestrings.h"
#include "scanner.h"

/* avoid compiler warnings for unused variables */
#define UNUSED(v)                                                       \
    do                                                                  \
    {                                                                   \
        while (0)                                                       \
        {                                                               \
            printf ("%p", (void *)&v);                                  \
        }                                                               \
    } while (0)


int
main (int argc, char **argv)
{
    UNUSED (argc);
    UNUSED (argv);

    char *crontab_path_raw = path_append (DEFAULT_CONFIG_DIR, CONFIG_FILENAME);
    char *crontab_path = expand_envvars (crontab_path_raw);
    scanner_t scanner = { 0 };
    symbols_t symbols = { 0 };
    size_t i = 0;
    int eof_flag = 0;

    printf ("crontab: %s %s\n", crontab_path_raw, crontab_path);

    scanner_init (&scanner, crontab_path);

    while (!eof_flag)
    {
        symbols = scanner_scan (&scanner);

        printf ("sym: ");
        for (i = 0; i < symbols.cnt; i++)
        {
            switch (symbols.m[i])
            {
            case SYM_NUMBER:
                printf ("SYM_NUMBER(%d), ", scanner.number);
                break;

            case SYM_STRING:
                printf ("SYM_STRING('%s'), ", scanner.string.m);
                break;

            case SYM_GENERIC:
                printf ("SYM_GENERIC, ");
                break;

            case SYM_COMMENT:
                printf ("SYM_COMMENT('%s'), ", scanner.string.m);
                break;

            case SYM_NEWLINE:
                printf ("SYM_NEWLINE, ");
                break;

            case SYM_EOF:
                printf ("SYM_EOF, ");
                eof_flag = 1;
                break;

            case SYM_UNKNOWN:
            default:
                printf ("SYM_UNKNOWN, ");
                break;
            }
        }
        putchar ('\n');
    }

    scanner_destroy (&scanner);
    free (crontab_path);
    free (crontab_path_raw);
    return 0;
}

/* newline = '\n' | '\r'
 *
 * comment = #[^newline]* newline
 *
 * digit = 0 | 1 | 2 | 3 | ... | 9
 * number = digit[digit...]
 *
 * generic = '*'
 *
 * number_param = number | generic
 * minute       = number_param
 * hour         = number_param
 * day_of_month = number_param
 * month        = number_param
 * day_of_week  = number_param
 *
 * command = [^newline]*
 *
 * statement = minute hour day_of_month month day_of_week command newline
 *
 * line = comment | statement
 */

/* end of file */
