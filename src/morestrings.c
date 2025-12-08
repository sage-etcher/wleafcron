
#include "morestrings.h"

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "imath.h"
#include "log.h"



#ifndef strdup
char *
strdup (const char *src) /* NOLINT */
{
    char *dst = NULL;

    dst = malloc (strlen (src) + 1);
    if (dst == NULL)
    {
        LOG_F ("failed memory allocation");
        return NULL;
    }

    (void)strcpy (dst, src);
    return dst;
}
#endif

size_t
string_get_size (size_t required)
{
    return (size_t)1 << log2sz (required);
}

int
string_init (struct string *self, size_t initial_size)
{
    int rc = 0;
    if (self == NULL)
    {
        return (errno = EINVAL);
    }

    self->m = NULL;
    self->alloc = 0;
    self->count = 0;
    rc = string_extend (self, string_get_size (initial_size));
    if (rc || self->m == NULL)
    {
        LOG_F ("failed string extend");
        return rc;
    }

    *self->m = '\0';

    return 0;
}

int
string_extend (struct string *self, size_t min_size)
{
    size_t new_alloc = 0;
    void *tmp = NULL;

    if (self == NULL)
    {
        return (errno = EINVAL);
    }

    if (min_size <= self->alloc)
    {
        return 0;
    }

    new_alloc = string_get_size (min_size);
    tmp = realloc (self->m, new_alloc);
    if (tmp == NULL)
    {
        LOG_E ("failed realloc");
        return errno;
    }

    self->m = tmp;
    self->alloc = new_alloc;

    return 0;
}

int
string_append (struct string *self, const char *child)
{
    size_t child_len = 0;
    size_t new_count = 0;

    if ((self == NULL) || (child == NULL))
    {
        return (errno = EINVAL);
    }

    child_len = strlen (child);
    new_count = self->count + child_len;

    string_extend (self, new_count+1);

    (void)memmove (&self->m[self->count], child, child_len);
    self->count = new_count;
    self->m[self->count] = '\0';

    return 0;
}

int
string_append_substring (struct string *self, char *child, size_t child_len)
{
    int rc = 0;
    char c = '\0';

    if ((self == NULL) || (child == NULL))
    {
        return (errno = EINVAL);
    }

    /* temporarily add null terminator */
    c = child[child_len];
    child[child_len] = '\0';

    /* append the string */
    rc = string_append (self, child);

    /* restore the character */
    child[child_len] = c;
    return rc;
}

char *
string_join (char **arr, size_t n, const char *seperator)
{
    int rc = 0;
    size_t i = 0;
    struct string res = { 0 };
    rc |= string_init (&res, 0);

    for (; i < n; i++)
    {
        rc |= string_append (&res, arr[i]);

        if (i + 1 < n)
        {
            rc |= string_append (&res, seperator);
        }
    }

    if (rc)
    {
        LOG_F ("failed string operation");
        free (res.m);
        return NULL;
    }

    return res.m;
}

/* end of file */
