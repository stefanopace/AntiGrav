#pragma once

#include <pebble.h>

void compact_clock_init(Layer *hours_parent, Layer *minutes_parent);
void compact_clock_deinit(void);
void compact_clock_set_visible(bool hours, bool minutes);
#if PBL_DISPLAY_WIDTH == 260 && PBL_DISPLAY_HEIGHT == 260
void compact_clock_shift_hours(int16_t shift_x, int16_t shift_y);
void compact_clock_shift_minutes(int16_t shift_x);
#endif
