
#ifndef LOG_H
#define LOG_H

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

#define LOG_PREFIX "wleafcron"

enum {
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_VERBOSE,
    LOG_DEBUG,
};

extern int g_log_level;

static inline int
flogf_s (FILE *fp, int level, const char *log_type, const char *fn_name, 
        const char *fmt, ...)
{
    int rc = errno;
    va_list args;

    if (level > g_log_level)
    {
        return rc;
    }

    va_start (args, fmt);

    (void)fprintf (fp, "%s: %s: ", LOG_PREFIX, log_type);
    if (g_log_level >= LOG_DEBUG)
    {
        (void)fprintf (fp, "%s(): ", fn_name);
    }
    (void)vfprintf (fp, fmt, args);
    (void)putc ('\n', fp);
    (void)fflush (fp);

    va_end (args);
    return (errno = rc);
}

#define LOG_F(...) flogf_s (stderr, LOG_FATAL,   "fatal",   __func__, __VA_ARGS__)
#define LOG_E(...) flogf_s (stderr, LOG_ERROR,   "error",   __func__, __VA_ARGS__)
#define LOG_W(...) flogf_s (stderr, LOG_WARNING, "warning", __func__, __VA_ARGS__)
#define LOG_D(...) flogf_s (stderr, LOG_DEBUG,   "debug",   __func__, __VA_ARGS__)
#define LOG_V(...) flogf_s (stdout, LOG_VERBOSE, "verbose", __func__, __VA_ARGS__)
#define LOG_I(...) flogf_s (stdout, LOG_INFO,    "info",    __func__, __VA_ARGS__)

#endif
/* end of file */
