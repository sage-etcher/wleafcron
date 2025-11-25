
#include "win_osctrl.h"

#if defined _WIN32

#include "commonos.h"
#include "morestrings.h"

#include <stdio.h>
#include <time.h>
#include <windows.h>

static int win_file_exists (const char *filepath);
static char *win_generate_cmdline (int argc, char **argv);

static int
win_file_exists (const char *filepath)
{
    FILE *fp = fopen (filepath, "r");

    if (fp == NULL) return 0;

    fclose (fp);
    return 1;
}

static char *
win_find_program_on_path (const char *path, const char *program)
{
    char *path_copy = strdup (path);
    char *program_path = NULL;

    char *iter = strtok (path_copy, ";");
    while (iter != NULL)
    {
        program_path = path_append (iter, program);
        if (win_file_exists (program_path))
        {
            return program_path;
        }

        free (program_path);
        iter = strtok (NULL, ";");
    }

    return NULL;
}

static char *
win_find_program (const char *program)
{
    if (win_file_exists (program)) 
    {
        return strdup (program);
    }

    return win_find_program_on_path (getenv ("PATH"), program);
}

static char *
win_generate_cmdline (int argc, char **argv)
{
    int i = 0;
    struct string cmdline = { 0 };
    string_init (&cmdline, 0);

    for (i = 0; i < argc; i++)
    {
        string_append (&cmdline, "\"");
        string_append (&cmdline, argv[i]);
        string_append (&cmdline, "\"");

        if (i + 1 != argc)
        {
            string_append (&cmdline, " ");
        }
    }

    return cmdline.m;
}

int
win_os_execute (int argc, char **argv)
{
    char *program = win_find_program (argv[0]);
    char *cmdline = win_generate_cmdline (argc-1, argv+1);
    PROCESS_INFORMATION process_info = { 0 };
    STARTUPINFOA info = { .cb = sizeof (info) };

    if (CreateProcessA (program, cmdline, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, 
                NULL, NULL, &info, &process_info))
    {
        //WaitForSingleObject (process_info.hProcess, INFINITE);
        CloseHandle (process_info.hProcess);
        CloseHandle (process_info.hThread);
    }

    free (cmdline);
    free (program);
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
