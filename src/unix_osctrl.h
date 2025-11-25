
#ifndef UNIX_OSCTRL_H
#define UNIX_OSCTRL_H

#include <time.h>

int unix_os_execute (int argc, char **argv);
time_t unix_os_fmtime (const char *filename);
void unix_os_sleep (unsigned seconds);

#endif
/* end of file */
