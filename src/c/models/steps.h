#pragma once
#include <pebble.h>

typedef struct StepsState
{
    int steps_amount;
    int steps_goal;
    bool goal_reached;
} StepsState;

int ag_steps_subscribe(void (*on_change)(const StepsState *state));
int ag_steps_unsubscribe(int subscription_handle);
