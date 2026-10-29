#pragma once

#include <pebble.h>

void pm_marker_init(Layer *parent);
void pm_marker_deinit(void);
void pm_marker_set_offset_x(int16_t offset);
