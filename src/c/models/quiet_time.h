#pragma once
#include <pebble.h>

typedef struct QuietTimeState
{
    bool active;
} QuietTimeState;

int ag_quiet_time_subscribe(void (*on_change)(const QuietTimeState *state));
int ag_quiet_time_unsubscribe(int subscription_handle);
