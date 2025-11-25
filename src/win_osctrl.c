
#include "win_osctrl.h"

#if defined _WIN32

#include "morestrings.h"

#include <stdio.h>
#include <time.h>
#include <windows.h>

int
win_os_execute (int argc, char **argv)
{
    char *cmdline = string_join (argv, argc, " ");
    PROCESS_INFORMATION process_info = { 0 };
    STARTUPINFOA info = { .cb = sizeof (info) };

    if (CreateProcessA (NULL, cmdline, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, 
                NULL, NULL, &info, &process_info))
    {
        //WaitForSingleObject (process_info.hProcess, INFINITE);
        CloseHandle (process_info.hProcess);
        CloseHandle (process_info.hThread);
    }

    free (cmdline);
    return 0;
}

time_t
win_os_fmtime (const char *filename)
{
    HANDLE h_file = NULL;
    FILETIME   mtime_file = { 0 };
    SYSTEMTIME mtime_system = { 0 };
    struct tm  mtime_tm = { 0 };
    time_t     mtime = 0;

    h_file = CreateFile (filename, GENERIC_READ, FILE_SHARE_READ, NULL, 
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    GetFileTime (h_file, NULL, NULL, &mtime_file);
    FileTimeToSystemTime (&mtime_file, &mtime_system);
    CloseHandle (h_file);

    mtime_tm = (struct tm){
        .tm_year = mtime_system.wYear - 1900,
        .tm_mon  = mtime_system.wMonth - 1,
        .tm_wday = mtime_system.wDayOfWeek,
        .tm_mday = mtime_system.wDay,
        .tm_hour = mtime_system.wHour,
        .tm_min  = mtime_system.wMinute,
        .tm_sec  = mtime_system.wSecond,
    };

    mtime = mktime (&mtime_tm);

    return mtime;
}

void
win_os_sleep (unsigned seconds)
{
    Sleep (seconds * 1000);
}

#endif
/* end of file */
