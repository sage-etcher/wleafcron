
#include "unix_osctrl.h"

#if defined __unix__

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "commonos.h"
#include "log.h"
#include "morestrings.h"

#include <unistd.h>
#include <sys/stat.h>

static int unix_file_is_executable (const char *filepath);
static char *unix_find_program_on_path (const char *path, const char *program);
static char *unix_find_program (const char *program);
static char **unix_terminate_str_array (int n, char **src);


static int
unix_file_is_executable (const char *filepath)
{
    return (!access (filepath, F_OK) && !access (filepath, X_OK));
}

static char *
unix_find_program_on_path (const char *path, const char *program)
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
        LOG_E ("path_copy strdup failure");
        return NULL;
    }

    char *iter = strtok (path_copy, ":");
    while (iter != NULL)
    {
        program_path = path_append (iter, program);
        if (program_path == NULL)
        {
            LOG_E ("program_path path_append failure");
            return NULL;
        }

        if (unix_file_is_executable (program_path))
        {
            return program_path;
        }

        free (program_path);
        iter = strtok (NULL, ":");
    }

    return NULL;
}

static char *
unix_find_program (const char *program)
{
    if (program[0] == '/' && unix_file_is_executable (program)) 
    {
        return strdup (program);
    }

    return unix_find_program_on_path (getenv ("PATH"), program);
}

static char **
unix_terminate_str_array (int n, char **src)
{
    char **dst = NULL;
    int i = 0;

    if (src == NULL)
    {
        errno = EINVAL;
        return NULL;
    }

    dst = (char **)malloc ((n + 1) * sizeof (*dst));
    if (dst == NULL)
    {
        LOG_E ("dst memory allocation failure");
        return NULL;
    }

    for (i = 0; i < n; i++)
    {
        dst[i] = strdup (src[i]);
        if (dst[i] == NULL)
        {
            LOG_E ("dst[%d] strdup failure", i);
            while (--i >= 0)
            {
                free (dst[i]);
            }
            free ((void *)dst);
            return NULL;
        }
    }
    dst[i] = NULL;

    return dst;
}

int 
unix_os_execute (int argc, char **argv)
{
    int i = 0;
    char *prog_path = NULL; 
    char **new_argv = NULL; 
    int child_pid = 0;

    if (argv == NULL)
    {
        return (errno = EINVAL);
    }

    child_pid = fork ();

    if (child_pid == 0)
    {
        new_argv = unix_terminate_str_array (argc, argv);
        prog_path = unix_find_program (argv[0]);
        if (prog_path == NULL)
        {
            LOG_E ("cannot find %s", argv[0]);
            exit (EXIT_FAILURE);
        }

        LOG_V ("starting %s", prog_path);
        for (i = 0; i < argc; i++)
        {
            LOG_V ("\targ #%d: %s", i, new_argv[i]);
        }

        execve (prog_path, new_argv, NULL);
        exit (EXIT_SUCCESS);
    }
    else if (child_pid > 0)
    {
        return 0;
    }
    else if (child_pid < 0)
    {
        LOG_E ("cannot start child process");
        return -1;
    }

    return -1;
}

time_t
unix_os_fmtime (const char *filename)
{
    struct stat attr = { 0 };

    if (stat (filename, &attr) == 0)
    {
        return attr.st_mtime;
    }

    return 0;
}

void
unix_os_sleep (unsigned seconds)
{
    (void)sleep (seconds);
}

#endif
/* end of file */
