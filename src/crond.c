
#include "crond.h"

#include <stdio.h>
#include <time.h>

#include "log.h"
#include "osctrl.h"
#include "rules.h"


int
crond (const char *crontab)
{
    int rc = 0;
    rules_t rules = { 0 };
    time_t now_time = 0;
    struct tm *now_tm = NULL;

    time_t oh = 0;
    time_t last_modified = os_fmtime (crontab);

    rc = rules_init (&rules);
    if (rc)
    {
        LOG_E ("failure initializing rules");
        return rc;
    }

    rc = rules_parse (&rules, crontab);
    if (rc)
    {
        LOG_E ("failure while parsing rules");
        rules_destroy (&rules);
        return rc;
    }

    while (1)
    {
        now_time = time (NULL);
        now_tm = localtime (&now_time);

        oh = os_fmtime (crontab);
        if (oh != last_modified)
        {
            LOG_I ("crontab updated, reloading - %s", crontab);

            rules_destroy (&rules);

            rc = rules_init (&rules);
            if (rc)
            {
                LOG_E ("failure reinitializing rules");
                return rc;
            }

            rc = rules_parse (&rules, crontab);
            if (rc)
            {
                LOG_E ("failure while reparsing rules");
                rules_destroy (&rules);
                return rc;
            }

            last_modified = oh;
        }

        if (rules_execute (&rules, now_tm))
        {
            LOG_E ("failure in rule execution");
        }

        os_sleep (1);
    }

    rules_destroy (&rules);

    return 0;
}

/* end of file */
