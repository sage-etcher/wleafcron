
#include "custom_getopt.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

char *c_optarg = NULL;
int c_optind = 1;
int c_opterr = 1;
int c_optopt = 0;

static int s_nextchar = 0;

int
c_getopt (int argc, char *argv[], const char *optstring)
{
    char c = 0;
    char *opt_c = NULL;

    if (argc <= c_optind) return -1;

    /* skip newlines */
    c = argv[c_optind][s_nextchar++];
    while ((c == '\0') && (argc > c_optind))
    {
        c_optind++;
        s_nextchar = 0;
        if (argc <= c_optind) return -1;
        c = argv[c_optind][s_nextchar++];
    }

    if (argv[c_optind][0] != '-') return -1;
    if ((argv[c_optind][1] == '-') && (argv[c_optind][2] == '\0')) return -1;


    /* get the next option */
    if (s_nextchar == 1)
    {
        c = argv[c_optind][s_nextchar++];
    }

    if (argc <= c_optind) return -1;

    /* lookup option in optstring */
    opt_c = strchr (optstring, c);
    if (opt_c == NULL)
    {
        c_optopt = (int)c;
        if (c_opterr)
        {
            (void)fprintf (stderr, "error: unrecognized option -%c\n", c_optopt);
        }
        return '?';
    }

    /* get option arguement if relevant */
    c_optarg = NULL;
    if (opt_c[1] == ':')
    {
        if (argv[c_optind][s_nextchar] != '\0')
        {
            c_optarg = &argv[c_optind][s_nextchar];
        }
        else
        {
            c_optarg = argv[++c_optind];
        }

        c_optind++;
        s_nextchar = 0;
    }

    return c;
}

/* end of file */
