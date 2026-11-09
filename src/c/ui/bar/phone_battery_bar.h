#pragma once
#include <pebble.h>

void phone_battery_bar_init(char slots_mask, GRect frame);
void phone_battery_bar_deinit();
void phone_battery_bar_mask_include(char bits);
void phone_battery_bar_mask_exclude(char bits);
