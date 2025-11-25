
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "commonos.h"
#include "config.h"
#include "crond.h"

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
    int rc = crond (crontab_path);

    free (crontab_path);
    free (crontab_path_raw);
    return rc;
}

/* end of file */
