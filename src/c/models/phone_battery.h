#pragma once
#include <pebble.h>

typedef struct PhoneBatteryState
{
    int percent;
    bool is_charging;
    bool is_low;
    bool available;
} PhoneBatteryState;

void ag_phone_battery_init();
void ag_phone_battery_deinit();
void ag_phone_battery_request();
int ag_phone_battery_subscribe(void (*on_change)(const PhoneBatteryState *state));
int ag_phone_battery_unsubscribe(int subscription_handle);
