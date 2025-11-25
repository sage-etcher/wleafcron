
#ifndef WIN_OSCTRL_H
#define WIN_OSCTRL_H

#include <time.h>

int win_os_execute (int argc, char **argv);
time_t win_os_fmtime (const char *filename);
void win_os_sleep (unsigned seconds);

#endif
/* end of file */
