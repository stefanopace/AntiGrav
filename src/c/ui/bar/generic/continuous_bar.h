#pragma once
#include <pebble.h>

void continuous_plain_bar_render(Layer *layer, GContext *ctx, int value, int scale, char slot_mask, bool available, bool mirrored, GColor color);
void continuous_combo_bar_render(GContext *ctx, int bar_y, int value, int scale, bool available, bool mirrored, GColor color, char slot_mask);