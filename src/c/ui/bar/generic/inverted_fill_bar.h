#pragma once
#include <pebble.h>

void inverted_fill_bar_render(Layer *layer, GContext *ctx, int value, int scale, char slot_mask, bool available, bool mirrored, GColor color);
