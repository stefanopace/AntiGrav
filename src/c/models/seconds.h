#pragma once
#include <pebble.h>

#define SECONDS_TEXT_SIZE 3

typedef struct SecondsState
{
    bool active;
    int amount;
    char text[SECONDS_TEXT_SIZE];
} SecondsState;

int ag_seconds_subscribe(void (*on_change)(const SecondsState *state));
int ag_seconds_unsubscribe(int subscription_handle);
