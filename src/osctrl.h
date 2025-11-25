
#ifndef OSCNTRL_H
#define OSCNTRL_H

#if defined __unix__
#include "unix_osctrl.h"

#define os_execute unix_os_execute
#define os_sleep   unix_os_sleep
#define os_fmtime  unix_os_fmtime

#elif defined _WIN32
#include "win_osctrl.h"

#define os_execute win_os_execute
#define os_sleep   win_os_sleep
#define os_fmtime  win_os_fmtime
#endif

#endif
/* end of file */
