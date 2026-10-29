#pragma once

#include <pebble.h>

void big_clock_init(Layer *hours_parent, Layer *minutes_parent);
void big_clock_deinit(void);
void big_clock_set_visible(bool hours, bool minutes);
void big_clock_set_offset_x(int16_t offset);
