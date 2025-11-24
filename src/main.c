
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "commonos.h"
#include "config.h"
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

    size_t ii = 0;
    size_t jj = 0;

    char *crontab_path_raw = path_append (DEFAULT_CONFIG_DIR, CONFIG_FILENAME);
    char *crontab_path = expand_envvars (crontab_path_raw);

    rules_t rules = { 0 };
    rules_init (&rules);
    rules_parse (&rules, crontab_path);

    for (ii = 0; ii < rules.count; ii++)
    {
        printf ("%d %d %d %d %d ",
                rules.m[ii].time[RULE_MINUTE],
                rules.m[ii].time[RULE_HOUR],
                rules.m[ii].time[RULE_MONTH_DAY],
                rules.m[ii].time[RULE_MONTH],
                rules.m[ii].time[RULE_WEEK_DAY]);

        for (jj = 0; jj < rules.m[ii].count; jj++)
        {
            printf ("'%s' ", rules.m[ii].command[jj]);
        }

        putchar ('\n');
    }

    rules_destroy (&rules);
    free (crontab_path);
    free (crontab_path_raw);
    return 0;
}

/* end of file */
