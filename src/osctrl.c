
#include "osctrl.h"

#include <stdio.h>


int
os_execute (int argc, char **argv)
{
    int i = 0;
    for (; i < argc; i++)
    {
        printf ("'%s' ", argv[i]);
    }
    putchar ('\n');

    return 0;
}

/* end of file */
