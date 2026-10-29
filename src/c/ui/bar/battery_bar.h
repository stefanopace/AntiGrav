#pragma once
#include <pebble.h>

void battery_bar_init(char slots_mask, GRect frame);
void battery_bar_deinit();