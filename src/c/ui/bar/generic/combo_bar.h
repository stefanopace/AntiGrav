#pragma once
#include <pebble.h>
#include "../../../settings.h"

void combo_bar_render(Layer *layer, GContext *ctx, int top_value, int top_scale, bool top_available, BarStyle top_style, int top_bar_type, bool top_mirrored, GColor top_color, int bottom_value, int bottom_scale, bool bottom_available, BarStyle bottom_style, int bottom_bar_type, bool bottom_mirrored, GColor bottom_color, char slot_mask);
