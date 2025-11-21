
#ifndef COMMONOS_H
#define COMMONOS_H

char *path_append (const char *parent, const char *child);
char *expand_envvars (const char *src);
char *readfile (const char *filename);

#endif
/* end of file */
