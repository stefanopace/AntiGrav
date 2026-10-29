#pragma once
#include <pebble.h>

int ag_tick_subscribe(void (*on_change)(const struct tm *tick), TimeUnits update_frequency);
int ag_tick_unsubscribe(int subscription_handle);
