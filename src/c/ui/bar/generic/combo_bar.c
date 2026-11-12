#include <pebble.h>
#include "combo_bar.h"
#include "../../layout.h"
#include "segmented_bar.h"
#include "continuous_bar.h"
#include "../../../settings.h"

typedef enum
{
    TOP,
    BOTTOM
} ComboBarPosition;

static void half_bar_render(Layer *layer, GContext *ctx, ComboBarPosition position, BarStyle battery_style, int value, int scale, bool available, int bar_type, bool mirrored, GColor color, char slot_mask);

void combo_bar_render(Layer *layer, GContext *ctx, int top_value, int top_scale, bool top_available, BarStyle top_style, int top_bar_type, bool top_mirrored, GColor top_color, int bottom_value, int bottom_scale, bool bottom_available, BarStyle bottom_style, int bottom_bar_type, bool bottom_mirrored, GColor bottom_color, char slot_mask)
{
    if (slot_mask == 0)
    {
        return;
    }

    half_bar_render(layer, ctx, TOP, top_style, top_value, top_scale, top_available, top_bar_type, top_mirrored, top_color, slot_mask);
    half_bar_render(layer, ctx, BOTTOM, bottom_style, bottom_value, bottom_scale, bottom_available, bottom_bar_type, bottom_mirrored, bottom_color, slot_mask);
}

static void half_bar_render(Layer *layer, GContext *ctx, ComboBarPosition position, BarStyle battery_style, int value, int scale, bool available, int bar_type, bool mirrored, GColor color, char slot_mask)
{
    const int bar_h = position == TOP ? COMBO_BAR_TOP_H : COMBO_BAR_BOTTOM_H;
    int bar_y = BAR_Y;
    if (position == BOTTOM)
    {
        bar_y = BAR_Y + COMBO_BAR_TOP_H + COMBO_BAR_GAP;
    }

    if (!available || battery_style == CONTINUOUS || battery_style == FILL || battery_style == INVERTED_FILL)
    {
        continuous_combo_bar_render(ctx, bar_y, bar_h, value, scale, available, mirrored, color, slot_mask);
    }
    else
    {
        segmented_combo_bar_render(layer, ctx, bar_y, bar_h, value, scale, available, bar_type, mirrored, color, slot_mask);
    }
}
