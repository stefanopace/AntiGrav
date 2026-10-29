#pragma once
#include <pebble.h>

typedef struct BatteryState
{
    int percent;
    bool is_charging;
    bool is_plugged;
    bool is_low;
} BatteryState;

int ag_battery_subscribe(void (*on_change)(const BatteryState *state));
int ag_battery_unsubscribe(int subscription_handle);
