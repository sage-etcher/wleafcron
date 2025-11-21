
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "commonos.h"
#include "config.h"
#include "parser.h"
#include "rules.h"

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
    parser_t parser = { 0 };
    rules_t *p_rules = &parser.rules;
    size_t ii = 0;
    size_t jj = 0;

    parser_init (&parser, crontab_path);

    parser_parse (&parser);
    for (ii = 0; ii < p_rules->count; ii++)
    {
        printf ("%d %d %d %d %d ",
                p_rules->m[ii].time[RULE_HOUR],
                p_rules->m[ii].time[RULE_MINUTE],
                p_rules->m[ii].time[RULE_MONTH_DAY],
                p_rules->m[ii].time[RULE_MONTH],
                p_rules->m[ii].time[RULE_WEEK_DAY]);

        for (jj = 0; jj < p_rules->m[ii].count; jj++)
        {
            printf ("'%s' ", p_rules->m[ii].command[jj]);
        }

        putchar ('\n');
    }



    parser_destroy (&parser);
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
