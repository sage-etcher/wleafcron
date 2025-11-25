
#ifndef MORESTRINGS_H
#define MORESTRINGS_H

#include <stddef.h>

struct string {
    char *m;
    size_t count;
    size_t alloc;
};

#ifndef strdup
char *strdup (const char *src);
#endif

int string_init (struct string *self, size_t initial_size);
int string_extend (struct string *self, size_t min_size);
int string_append (struct string *self, const char *child);
int string_append_substring (struct string *self, char *child, 
        size_t child_len);
char *string_join (char **arr, size_t n, const char *seperator);

size_t string_get_size (size_t required);

#endif
/* end of file */
