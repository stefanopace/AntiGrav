#pragma once
#include <pebble.h>

void battery_bar_init(char slots_mask, GRect frame);
void battery_bar_deinit();
void battery_bar_mask_include(char bits);
void battery_bar_mask_exclude(char bits);