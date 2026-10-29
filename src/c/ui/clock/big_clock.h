#pragma once

#include <pebble.h>

void big_clock_init(Layer *hours_parent, Layer *minutes_parent);
void big_clock_deinit(void);
void big_clock_set_visible(bool hours, bool minutes);
#if PBL_DISPLAY_WIDTH == 260 && PBL_DISPLAY_HEIGHT == 260
void big_clock_shift_hours(int16_t shift_x);
void big_clock_shift_minutes(int16_t shift_x, int16_t shift_y);
#endif
