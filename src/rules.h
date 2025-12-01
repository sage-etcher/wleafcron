
#ifndef RULES_H
#define RULES_H

#include <stddef.h>
#include <time.h>

enum {
    RULE_MINUTE,
    RULE_HOUR,
    RULE_MONTH_DAY,
    RULE_MONTH,
    RULE_WEEK_DAY,
    RULE_PARAM_COUNT,
};

enum {
    RULE_NOT_MATCHED,
    RULE_MATCHED,
};

typedef struct {
    int time[RULE_PARAM_COUNT];
    int status;
    char **command;
    size_t alloc;
    size_t count;
} singlerule_t;

typedef struct {
    singlerule_t *m;
    size_t alloc;
    size_t count;
} rules_t;

#define RULE_GENERIC (-1)
extern const int RULE_PARAM_MIN[RULE_PARAM_COUNT];
extern const int RULE_PARAM_MAX[RULE_PARAM_COUNT];
extern const char *RULE_PARAM_NAME[RULE_PARAM_COUNT];

int singlerule_init (singlerule_t *self);
void singlerule_destroy (singlerule_t *self);
int singlerule_add_arg (singlerule_t *self, char *new_arg);
int singlerule_match (singlerule_t *self, struct tm *now);

int rules_init (rules_t *self);
void rules_destroy (rules_t *self);
int rules_add (rules_t *self, singlerule_t new_rule);
int rules_parse (rules_t *self, const char *filename);
int rules_execute (rules_t *self, struct tm *now);


#endif
/* end of file */
