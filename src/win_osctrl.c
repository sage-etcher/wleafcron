
#include "win_osctrl.h"

#if defined _WIN32

#include "commonos.h"
#include "log.h"
#include "morestrings.h"

#include <stdio.h>
#include <time.h>
#include <windows.h>

static int win_file_exists (const char *filepath);
static char *win_generate_cmdline (int argc, char **argv);

static int
win_file_exists (const char *filepath)
{
    FILE *fp = NULL;

    if (filepath == NULL)
    {
        errno = EINVAL;
        return 0;
    }

    fp = fopen (filepath, "r");
    if (fp == NULL) 
    {
        return 0;
    }

    (void)fclose (fp);
    return 1;
}

static char *
win_find_program_on_path (const char *path, const char *program)
{
    char *path_copy = NULL;
    char *program_path = NULL;

    if ((path == NULL) || (program == NULL))
    {
        errno = EINVAL;
        return NULL;
    }

    path_copy = strdup (path);
    if (path_copy == NULL)
    {
        LOG_E ("path_copy memory error");
        return NULL;
    }

    char *iter = strtok (path_copy, ";");
    while (iter != NULL)
    {
        program_path = path_append (iter, program);
        if (program_path == NULL)
        {
            LOG_E ("program_path memory error");
            return NULL;
        }

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
    if (program == NULL)
    {
        errno = EINVAL;
        return NULL;
    }

    if (win_file_exists (program)) 
    {
        return strdup (program);
    }

    return win_find_program_on_path (getenv ("PATH"), program);
}

static char *
win_generate_cmdline (int argc, char **argv)
{
    int rc = 0;
    int i = 0;
    struct string cmdline = { 0 };

    rc |= string_init (&cmdline, 0);

    for (i = 0; i < argc; i++)
    {
        rc |= string_append (&cmdline, "\"");
        rc |= string_append (&cmdline, argv[i]);
        rc |= string_append (&cmdline, "\"");

        if (i + 1 != argc)
        {
            rc |= string_append (&cmdline, " ");
        }
    }

    if (rc)
    {
        LOG_E ("cmdline string generation");
        free (cmdline.m);
        return NULL;
    }

    return cmdline.m;
}

int
win_os_execute (int argc, char **argv)
{
    char *program = NULL;
    char *cmdline = NULL; 
    PROCESS_INFORMATION process_info = { 0 };
    STARTUPINFOA info = { .cb = sizeof (info) };

    return 0;

    program = win_find_program (argv[0]);
    cmdline = win_generate_cmdline (argc-1, argv+1);

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

static int i = 0;

time_t
win_os_fmtime (const char *filename)
{
    HANDLE h_file = NULL;
    FILETIME   mtime_file = { 0 };
    SYSTEMTIME mtime_system = { 0 };
    struct tm  mtime_tm = { 0 };
    time_t     mtime = 0;

    return i++;

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
