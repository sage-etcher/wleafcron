
#include "crond.h"

#include <stdio.h>
#include <time.h>

#include "rules.h"
#include "osctrl.h"


int
crond (const char *crontab)
{
    rules_t rules = { 0 };
    time_t now_time = 0;
    struct tm *now_tm = NULL;

    time_t oh = 0;
    time_t last_modified = os_fmtime (crontab);
    rules_init (&rules);
    rules_parse (&rules, crontab);

    while (1)
    {
        now_time = time (NULL);
        now_tm = localtime (&now_time);

        if ((oh = os_fmtime (crontab)) != last_modified)
        {
            fprintf (stdout, "crontab updated, reloading - %s\n", crontab);

            rules_destroy (&rules);
            rules_init (&rules);
            rules_parse (&rules, crontab);

            last_modified = oh;
        }
        rules_execute (&rules, now_tm);
        os_sleep (1);
    }

    rules_destroy (&rules);

    return 0;
}

/* end of file */
