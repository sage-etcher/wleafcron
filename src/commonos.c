
#include "commonos.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "morestrings.h"

#if defined _WIN32
#   define DIR_SEPERATOR "\\"
#elif defined __unix__ 
#   define DIR_SEPERATOR "/"
#endif

char *
path_append (const char *parent, const char *child)
{
    struct string path = { 0 };

    /* null guard */
    if ((parent == NULL) || (child == NULL))
    {
        errno = EINVAL;
        return NULL;
    }

    string_init (&path, strlen (parent) + sizeof (char) + strlen (child));
    string_append (&path, parent);
    string_append (&path, DIR_SEPERATOR);
    string_append (&path, child);

    return path.m;
}

char *
expand_envvars (const char *src)
{
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

    /* get prefix */
    src_iter = strchr (src_cpy, '%');
    if (src_iter == NULL)
    {
        /* nothing to expand */
        return src_cpy;
    }
    substr_len = src_iter - src_cpy;

    string_init (&dst, substr_len);
    string_append_substring (&dst, src_cpy, substr_len);

    /* find first environment variable */
    src_iter = strtok (src_cpy, "%");
    while (src_iter != NULL)
    {
        /* append expanded environment variable */
        env_value = getenv (src_iter);
        string_append (&dst, env_value);

        /* append trailing plain text */
        src_iter = strtok (NULL, "%\0");    /* end of string or env */
        string_append (&dst, src_iter);

        /* find next environment variable */
        src_iter = strtok (NULL, "%");
    }

    free (src_cpy);
    return dst.m;
}

char *
readfile (const char *filename)
{
    FILE *fp = NULL;
    char *content = NULL;
    int size = 0;
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

    fseek (fp, 0L, SEEK_END);
    size = ftell (fp);

    fseek (fp, 0L, SEEK_SET);

    content = malloc (size + 1);
    if (content == NULL)
    {
        rc = errno;
        fclose (fp);
        errno = rc;
        return NULL;
    }

    fread (content, sizeof (char), size, fp);
    content[size] = '\0';

    fclose (fp);
    return content;
}

/* end of file */
