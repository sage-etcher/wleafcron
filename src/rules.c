
#include "rules.h"

#include <stdlib.h>

#include "parser.h"

const int RULE_PARAM_MIN[RULE_PARAM_COUNT] = {
    [RULE_MINUTE]    = 0,
    [RULE_HOUR]      = 0,
    [RULE_MONTH_DAY] = 1,
    [RULE_MONTH]     = 1,
    [RULE_WEEK_DAY]  = 1,
};
const int RULE_PARAM_MAX[RULE_PARAM_COUNT] = {
    [RULE_MINUTE]    = 59,
    [RULE_HOUR]      = 23,
    [RULE_MONTH_DAY] = 31,
    [RULE_MONTH]     = 12,
    [RULE_WEEK_DAY]  = 6,
};
const char *RULE_PARAM_NAME[RULE_PARAM_COUNT] = {
    [RULE_MINUTE]    = "Minute",
    [RULE_HOUR]      = "Hour",
    [RULE_MONTH_DAY] = "Day of the month",
    [RULE_MONTH]     = "Month of the year",
    [RULE_WEEK_DAY]  = "Day of the week",
};

int
singlerule_init (singlerule_t *self)
{
    singlerule_t new = (singlerule_t){
        .time = {
            [RULE_HOUR]      = RULE_GENERIC,
            [RULE_MINUTE]    = RULE_GENERIC,
            [RULE_MONTH_DAY] = RULE_GENERIC,
            [RULE_MONTH]     = RULE_GENERIC,
            [RULE_WEEK_DAY]  = RULE_GENERIC,
        },
        .command = malloc (sizeof (char *)),
        .alloc = 1,
        .count = 0,
    };

    if (new.command == NULL)
    {
        return 1;
    }

    *self = new;
    return 0;
}

int
singlerule_add_arg (singlerule_t *self, char *new_arg)
{
    size_t new_alloc = 0;
    void *tmp = NULL;

    if (self->count >= self->alloc)
    {
        new_alloc = self->alloc ? self->alloc * 2 : 1;
        tmp = realloc (self->command, new_alloc * sizeof (char *));
        if (tmp == NULL)
        {
            return 1;
        }

        self->command = tmp;
        self->alloc = new_alloc;
    }

    self->command[self->count] = new_arg;
    self->count++;

    return 0;
}

void 
singlerule_destroy (singlerule_t *self)
{
    size_t i = 0;

    for (; i < self->count; i++)
    {
        free (self->command[i]);
        self->command[i] = NULL;
    }
    free (self->command);
    self->command = NULL;
    self->alloc = 0;
    self->count = 0;
}

int
rules_init (rules_t *self)
{
    *self = (rules_t){ .m = NULL, .alloc = 0, .count = 0 };
    return 0;
}

int
rules_add (rules_t *self, singlerule_t new_rule)
{
    size_t new_alloc = 0;
    void *tmp = NULL;

    if (self->count >= self->alloc)
    {
        new_alloc = self->alloc ? self->alloc * 2 : 1;
        tmp = realloc (self->m, new_alloc * sizeof (singlerule_t));
        if (tmp == NULL)
        {
            return 1;
        }

        self->m= tmp;
        self->alloc = new_alloc;
    }

    self->m[self->count] = new_rule;
    self->count++;

    return 0;
}

void
rules_destroy (rules_t *self)
{
    size_t i = 0;

    for (; i < self->count; i++)
    {
        singlerule_destroy (&self->m[i]);
    }
    free (self->m);
    self->m = NULL;
    self->alloc = 0;
    self->count = 0;
}

int
rules_parse (rules_t *self, const char *filename)
{
    parser_t parser = { 0 };
    parser_init (&parser, filename, self);

    parser_parse (&parser);

    parser_destroy (&parser);
    return 0;
}


/* end of file */
