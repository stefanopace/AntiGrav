#pragma once

#include <pebble.h>

void pm_marker_init(Layer *parent);
void pm_marker_deinit(void);
void pm_marker_set_hours_big(bool is_big);
#if PBL_DISPLAY_WIDTH == 260 && PBL_DISPLAY_HEIGHT == 260
void pm_marker_shift(int16_t shift_x, int16_t shift_y);
#endif
