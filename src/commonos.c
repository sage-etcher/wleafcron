
#include "commonos.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "morestrings.h"

#if defined _WIN32
#   define DIR_SEPERATOR "\\"
#elif defined __unix__ 
#   define DIR_SEPERATOR "/"
#endif

#define MIN(x,y) ((x) < (y) ? (x) : (y))

char *
path_append (const char *parent, const char *child)
{
    int rc = 0;
    struct string path = { 0 };

    /* null guard */
    if ((parent == NULL) || (child == NULL))
    {
        errno = EINVAL;
        return NULL;
    }

    rc |= string_init (&path, strlen (parent) + sizeof (char) + strlen (child));
    rc |= string_append (&path, parent);
    rc |= string_append (&path, DIR_SEPERATOR);
    rc |= string_append (&path, child);

    if (rc != 0)
    {
        LOG_E ("string operation failed");
        free (path.m);
        return NULL;
    }

    return path.m;
}

char *
expand_envvars (const char *src)
{
    int rc = 0;
    char *src_cpy = NULL;
    char *src_iter = NULL;
    char *env_value = NULL;

    size_t substr_len = 0;
    struct string dst = { 0 };

    if (src == NULL)
    {
        errno = EINVAL;
        return NULL;
    }

    src_cpy = strdup (src);
    src_iter = src_cpy;
    if (src_cpy == NULL)
    {
        LOG_E ("str_cpy failure");
        return NULL;
    }

    /* get prefix */
    src_iter = strchr (src_cpy, '%');
    if (src_iter == NULL)
    {
        /* nothing to expand */
        return src_cpy;
    }
    substr_len = src_iter - src_cpy;

    rc |= string_init (&dst, substr_len);
    rc |= string_append_substring (&dst, src_cpy, substr_len);

    /* find first environment variable */
    src_iter = strtok (src_cpy, "%");
    while (src_iter != NULL)
    {
        /* append expanded environment variable */
        env_value = getenv (src_iter);
        rc |= string_append (&dst, env_value);

        /* append trailing plain text */
        src_iter = strtok (NULL, "%\0");    /* end of string or env */
        rc |= string_append (&dst, src_iter);

        /* find next environment variable */
        src_iter = strtok (NULL, "%");
    }

    if (rc != 0)
    {
        LOG_E ("string failure");
        free (src_cpy);
        free (dst.m);
        return NULL;
    }

    free (src_cpy);
    return dst.m;
}

char *
readfile (const char *filename)
{
    FILE *fp = NULL;
    char *content = NULL;
    size_t size = 0;
    size_t read_bytes = 0;
    int rc = 0;

    if (filename == NULL)
    {
        errno = EINVAL;
        return NULL;
    }

    fp = fopen (filename, "r");
    if (fp == NULL)
    {
        return NULL;
    }

    rc |= fseek (fp, 0L, SEEK_END);
    size = ftell (fp);
    rc |= fseek (fp, 0L, SEEK_SET);

    if (rc != 0)
    {
        LOG_E ("fseek failure");
        (void)fclose (fp);
        return NULL;
    }

    content = malloc (size + 1);
    if (content == NULL)
    {
        LOG_E ("content memory error");
        (void)fclose (fp);
        return NULL;
    }

    read_bytes = fread (content, sizeof (char), size, fp);
    if (read_bytes != size)
    {
        LOG_W ("ftell vs fread size mismatch");
    }
    content[MIN (size, read_bytes)] = '\0';

    (void)fclose (fp);
    return content;
}

/* end of file */
