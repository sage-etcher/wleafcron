
#include "rules.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "osctrl.h"
#include "parser.h"

const int RULE_PARAM_MIN[RULE_PARAM_COUNT] = {
    [RULE_MINUTE]    = 0,
    [RULE_HOUR]      = 0,
    [RULE_MONTH_DAY] = 1,
    [RULE_MONTH]     = 1,
    [RULE_WEEK_DAY]  = 0,
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
    singlerule_t new = { 0 };

    if (self == NULL)
    {
        return (errno = EINVAL);
    }

    new = (singlerule_t){
        .time = {
            [RULE_HOUR]      = RULE_GENERIC,
            [RULE_MINUTE]    = RULE_GENERIC,
            [RULE_MONTH_DAY] = RULE_GENERIC,
            [RULE_MONTH]     = RULE_GENERIC,
            [RULE_WEEK_DAY]  = RULE_GENERIC,
        },
        .status = RULE_NOT_MATCHED,
        .command = (char **)malloc (sizeof (char *)),
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

    if ((self == NULL) || (new_arg == NULL))
    {
        return (errno = EINVAL);
    }

    if (self->count >= self->alloc)
    {
        new_alloc = self->alloc ? self->alloc * 2 : 1;
        tmp = realloc ((void *)self->command, new_alloc * sizeof (char *));
        if (tmp == NULL)
        {
            return 1;
        }

        self->command = (char **)tmp;
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

    if (self == NULL)
    {
        return;
    }

    for (i = 0; i < RULE_PARAM_COUNT; i++)
    {
        self->time[i] = 0;
    }

    self->status = 0;

    for (i = 0; i < self->count; i++)
    {
        free (self->command[i]);
        self->command[i] = NULL;
    }
    free ((void *)self->command);
    self->command = NULL;
    self->alloc = 0;
    self->count = 0;

}

int
param_match (singlerule_t *self, size_t param, int now_obj)
{
    int x = self->time[param];

    if ((self == NULL) || (param >= RULE_PARAM_COUNT))
    {
        errno = EINVAL;
        return 0;
    }

    return (x == RULE_GENERIC || x == now_obj);
}

int
singlerule_match (singlerule_t *self, struct tm *now)
{
    if ((self == NULL) || (now == NULL))
    {
        errno = EINVAL;
        return 0;
    }

    return (param_match (self, RULE_MINUTE, now->tm_min) &&
            param_match (self, RULE_HOUR,   now->tm_hour) &&
            param_match (self, RULE_MONTH_DAY, now->tm_mday) &&
            param_match (self, RULE_MONTH, now->tm_mon) &&
            param_match (self, RULE_WEEK_DAY, now->tm_wday));
}

int
rules_init (rules_t *self)
{
    if (self == NULL)
    {
        return (errno = EINVAL);
    }

    *self = (rules_t){ .m = NULL, .alloc = 0, .count = 0 };
    return 0;
}

int
rules_add (rules_t *self, singlerule_t new_rule)
{
    size_t new_alloc = 0;
    void *tmp = NULL;

    if (self == NULL)
    {
        return (errno = EINVAL);
    }

    if (self->count >= self->alloc)
    {
        new_alloc = self->alloc ? self->alloc * 2 : 1;
        tmp = realloc (self->m, new_alloc * sizeof (singlerule_t));
        if (tmp == NULL)
        {
            LOG_E ("failed realloc");
            return 1;
        }

        self->m = tmp;
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

    if (self == NULL)
    {
        return;
    }

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
    int rc = 0;
    parser_t parser = { 0 };

    if ((self == NULL) || (filename == NULL))
    {
        return (errno = EINVAL);
    }

    rc = parser_init (&parser, filename, self);
    if (rc)
    {
        LOG_E ("failed to initialize a parser");
        return rc;
    }

    rc = parser_parse (&parser);
    if (rc)
    {
        LOG_E ("failed to parse file");
        parser_destroy (&parser);
        return rc;
    }

    parser_destroy (&parser);
    return 0;
}

int
rules_execute (rules_t *self, struct tm *now)
{
    size_t i = 0;
    singlerule_t *p_rule = NULL;

    if ((self == NULL) || (now == NULL))
    {
        return (errno = EINVAL);
    }

    for (i = 0; i < self->count; i++)
    {
        p_rule = &self->m[i];

        if (!singlerule_match (p_rule, now)) 
        {
            p_rule->status = RULE_NOT_MATCHED;
            continue;
        }

        if (p_rule->status == RULE_MATCHED) 
        {
            continue;
        }

        p_rule->status = RULE_MATCHED;
        (void)os_execute ((int)p_rule->count, p_rule->command);
    }

    return 0;
}

/* end of file */
