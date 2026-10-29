#pragma once
#include <pebble.h>
#include "../../layout.h"

enum
{
    SEGMENTED_BAR_FLOOR,
    SEGMENTED_BAR_CEIL
};

void segmented_bar_render(Layer *layer, GContext *ctx, int value, int scale, char slot_mask, int bar_type, bool available, int bar_y, int bar_h, int empty_h, bool mirrored, GColor color);