
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "commonos.h"
#include "config.h"
#include "crond.h"
#include "custom_getopt.h"
#include "log.h"
#include "morestrings.h"

/* avoid compiler warnings for unused variables */
#define UNUSED(v)                                                       \
    do                                                                  \
    {                                                                   \
        while (0)                                                       \
        {                                                               \
            printf ("%p", (void *)&v);                                  \
        }                                                               \
    } while (0)

int g_log_level = LOG_INFO;



int
main (int argc, char **argv)
{
    char *crontab_path_raw = NULL;
    char *crontab_path = NULL;
    int rc = 0;

    int c = 0;

    while (c != -1)
    {
        c = getopt (argc, argv, "f:ctvd");

        switch (c)
        {
        case -1: break; /* exit */

        case 'f': 
            free (crontab_path);
            crontab_path = strdup (optarg);
            break;

        case 'c': g_log_level = LOG_WARNING; break;
        case 't': g_log_level = LOG_INFO; break;
        case 'v': g_log_level = LOG_VERBOSE; break;
        case 'd': g_log_level = LOG_DEBUG; break;

        default:
            printf ("usage: wleafcron -ctvd -f cronttab\n");
            printf ("\n");
            printf ("options:\n");
            printf ("  -f crontab    use custom crontab file\n");
            printf ("  -c            log only warnings and error\n");
            printf ("  -t            log terse\n");
            printf ("  -v            log verbose information\n");
            printf ("  -d            log debug information\n");
            exit (EXIT_FAILURE);
        }
    }

    if (crontab_path == NULL)
    {
        crontab_path_raw = path_append (DEFAULT_CONFIG_DIR, CONFIG_FILENAME);
        crontab_path = expand_envvars (crontab_path_raw);
    }

    LOG_V ("log level - %d", g_log_level);
    LOG_V ("crontab - %s", crontab_path);

    rc = crond (crontab_path);

    free (crontab_path);
    free (crontab_path_raw);
    return rc;
}

/* end of file */
