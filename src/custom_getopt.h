
#ifndef CUSTOM_GETOPT
#define CUSTOM_GETOPT

#include <stdio.h>
#include <stdlib.h>

extern char *c_optarg;
extern int c_optind, c_opterr, c_optopt;

int c_getopt (int argc, char *argv[], const char *optstring);

#ifndef getopt
#   define getopt c_getopt
#   define optarg c_optarg
#   define optind c_optind
#   define opterr c_opterr
#   define optopt c_optopt
#endif

#endif
/* end of file */
