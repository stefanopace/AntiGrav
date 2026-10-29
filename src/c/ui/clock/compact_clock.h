#pragma once

#include <pebble.h>

void compact_clock_init(Layer *hours_parent, Layer *minutes_parent);
void compact_clock_deinit(void);
void compact_clock_set_visible(bool hours, bool minutes);
void compact_clock_set_offset_x(int16_t offset);
