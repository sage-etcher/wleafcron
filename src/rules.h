
#ifndef RULES_H
#define RULES_H

#include <stddef.h>

enum {
    RULE_HOUR,
    RULE_MINUTE,
    RULE_MONTH_DAY,
    RULE_MONTH,
    RULE_WEEK_DAY,
    RULE_PARAM_COUNT,
};

typedef struct {
    int time[RULE_PARAM_COUNT];
    char **command;
    size_t alloc;
    size_t count;
} singlerule_t;

typedef struct {
    singlerule_t *m;
    size_t alloc;
    size_t count;
} rules_t;

int singlerule_init (singlerule_t *self);
void singlerule_destroy (singlerule_t *self);
int singlerule_add_arg (singlerule_t *self, char *new_arg);

int rules_init (rules_t *self);
void rules_destroy (rules_t *self);
int rules_add (rules_t *self, singlerule_t new_rule);

#endif
/* end of file */
