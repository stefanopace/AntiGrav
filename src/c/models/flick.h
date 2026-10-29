#pragma once
#include <pebble.h>

int ag_flick_subscribe(void (*on_flick)(), int flick_count);
int ag_flick_unsubscribe(int subscription_handle);
