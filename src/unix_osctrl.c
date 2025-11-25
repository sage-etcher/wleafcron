
#include "unix_osctrl.h"

#if defined __unix__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "commonos.h"
#include "morestrings.h"

#include <unistd.h>
#include <sys/stat.h>


int unix_os_execute (int argc, char **argv);
time_t unix_os_fmtime (const char *filename);
void unix_os_sleep (unsigned seconds);

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
    char *path_copy = strdup (path);
    char *program_path = NULL;

    char *iter = strtok (path_copy, ":");
    while (iter != NULL)
    {
        program_path = path_append (iter, program);
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
    char **dst = malloc ((n + 1) * sizeof (*dst));
    int i = 0;

    for (i = 0; i < n; i++)
    {
        dst[i] = strdup (src[i]);
    }
    dst[i+1] = NULL;

    return dst;
}

int 
unix_os_execute (int argc, char **argv)
{
    char *prog_path = NULL; 
    char **new_argv = NULL; 
    int child_pid = fork ();

    if (child_pid == 0)
    {
        new_argv = unix_terminate_str_array (argc, argv);
        prog_path = unix_find_program (argv[0]);
        if (prog_path == NULL)
        {
            fprintf (stderr, "error: cannot find %s\n", argv[0]);
            exit (0);
        }

        execve (prog_path, new_argv, NULL);
        exit (0);
    }
    else if (child_pid > 0)
    {
        return 0;
    }
    else if (child_pid < 0)
    {
        fprintf (stderr, "error: cannot start child process\n");
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
    sleep (seconds);
}

#endif
/* end of file */
