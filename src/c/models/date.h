#pragma once

#include <pebble.h>

#define DATE_TEXT_SIZE 3
#define DATE_YEAR_SIZE 5

typedef struct DateState
{
    char day[DATE_TEXT_SIZE];
    char month[DATE_TEXT_SIZE];
    char year[DATE_YEAR_SIZE];
    int weekday;
    int month_index;
} DateState;

int ag_date_subscribe(void (*on_change)(const DateState *state));
int ag_date_unsubscribe(int subscription_handle);
