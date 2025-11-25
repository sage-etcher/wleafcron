
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "commonos.h"
#include "config.h"
#include "osctrl.h"
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

    rules_t rules = { 0 };
    time_t now_time = 0;
    struct tm *now_tm = NULL;

    time_t oh = 0;
    time_t last_modified = os_fmtime (crontab_path);
    rules_init (&rules);
    rules_parse (&rules, crontab_path);

    while (1)
    {
        now_time = time (NULL);
        now_tm = localtime (&now_time);

        if ((oh = os_fmtime (crontab_path)) != last_modified)
        {
            fprintf (stdout, "crontab updated, reloading - %s\n", crontab_path);

            rules_destroy (&rules);
            rules_init (&rules);
            rules_parse (&rules, crontab_path);

            last_modified = oh;
        }
        rules_execute (&rules, now_tm);
        os_sleep (1);
    }

    rules_destroy (&rules);
    free (crontab_path);
    free (crontab_path_raw);
    return 0;
}




/* end of file */
