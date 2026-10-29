#include <pebble.h>
#include "combo_bar.h"
#include "../../layout.h"
#include "segmented_bar.h"
#include "../../../settings.h"

typedef enum
{
    TOP,
    BOTTOM
} ComboBarPosition;

static void half_bar_render(Layer *layer, GContext *ctx, ComboBarPosition position, BarStyle battery_style, int value, int scale, bool available, int bar_type, bool mirrored, GColor color, char slot_mask);
static void half_bar_render_continuous(Layer *layer, GContext *ctx, int bar_y, int bar_h, int value, int scale, bool available, bool mirrored, GColor color, char slot_mask);
static void half_bar_render_segmented(Layer *layer, GContext *ctx, int bar_y, int bar_h, int value, int scale, bool available, int bar_type, bool mirrored, GColor color, char slot_mask);

void combo_bar_render(Layer *layer, GContext *ctx, int top_value, int top_scale, bool top_available, BarStyle top_style, int top_bar_type, bool top_mirrored, GColor top_color, int bottom_value, int bottom_scale, bool bottom_available, BarStyle bottom_style, int bottom_bar_type, bool bottom_mirrored, GColor bottom_color, char slot_mask)
{
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

    if (battery_style == CONTINUOUS || !available)
    {
        half_bar_render_continuous(layer, ctx, bar_y, bar_h, value, scale, available, mirrored, color, slot_mask);
    }
    else
    {
        half_bar_render_segmented(layer, ctx, bar_y, bar_h, value, scale, available, bar_type, mirrored, color, slot_mask);
    }
}

static void half_bar_render_continuous(Layer *layer, GContext *ctx, int bar_y, int bar_h, int value, int scale, bool available, bool mirrored, GColor color, char slot_mask)
{
    if (!available)
    {
        value = 1;
        scale = 1;
        bar_h = 1;
        bar_y = bar_y + 1;
    }

    bool jump = false;
    int x = BAR1_X;
    int bar_width = BAR_W;
    bool starts_left_border = false;
    bool ends_right_border = false;

    switch (slot_mask)
    {
    case 1:
        x = BAR3_X;
        ends_right_border = true;
        break;
    case 2:
        x = BAR2_X;
        break;
    case 3:
        x = BAR2_X;
        bar_width = 2 * BAR_W + BAR_GAP;
        ends_right_border = true;
        break;
    case 4:
        starts_left_border = true;
        break;
    case 5:
        jump = true;
        bar_width = 2 * BAR_W;
        starts_left_border = true;
        ends_right_border = true;
        break;
    case 6:
        bar_width = 2 * BAR_W + BAR_GAP;
        starts_left_border = true;
        break;
    case 7:
        bar_width = 3 * BAR_W + 2 * BAR_GAP;
        starts_left_border = true;
        ends_right_border = true;
        break;
    default:
    }

    int max_len = bar_width;
    int actual_len = 0;
    if (value > 0 && scale > 0)
    {
        if (value >= scale)
        {
            actual_len = max_len;
        }
        else
        {
            actual_len = (int)(((long long)max_len * value) / scale);
        }
    }

    graphics_context_set_fill_color(ctx, color);

    if (jump)
    {
        int max_split_len = max_len / 2;
        int first_half_len = 0;
        int second_half_len = 0;
        if (actual_len > max_split_len)
        {
            first_half_len = max_split_len;
            second_half_len = actual_len - max_split_len;
        }
        else
        {
            first_half_len = actual_len;
            second_half_len = 0;
        }

        if (!mirrored)
        {
            graphics_fill_rect(
                ctx,
                GRect(0, bar_y, first_half_len, bar_h),
                BAR_CORNER_RADIUS,
                GCornersLeft);

            graphics_fill_rect(
                ctx,
                GRect(BAR3_X, bar_y, second_half_len, bar_h),
                BAR_CORNER_RADIUS,
                GCornersRight);
        }
        else
        {
            graphics_fill_rect(
                ctx,
                GRect(max_split_len - second_half_len, bar_y, second_half_len, bar_h),
                BAR_CORNER_RADIUS,
                GCornersLeft);

            graphics_fill_rect(
                ctx,
                GRect(BAR3_X + max_split_len - first_half_len, bar_y, first_half_len, bar_h),
                BAR_CORNER_RADIUS,
                GCornersRight);
        }
    }
    else
    {
        GCornerMask corners = GCornerNone;
        if (starts_left_border && ends_right_border)
        {
            corners = GCornersAll;
        }
        else if (starts_left_border)
        {
            corners = GCornersLeft;
        }
        else if (ends_right_border)
        {
            corners = GCornersRight;
        }

        if (!mirrored)
        {
            graphics_fill_rect(
                ctx,
                GRect(x, bar_y, actual_len, bar_h),
                BAR_CORNER_RADIUS,
                corners);
        }
        else
        {
            graphics_fill_rect(
                ctx,
                GRect(x + max_len - actual_len, bar_y, actual_len, bar_h),
                BAR_CORNER_RADIUS,
                corners);
        }
    }
}

static void half_bar_render_segmented(Layer *layer, GContext *ctx, int bar_y, int bar_h, int value, int scale, bool available, int bar_type, bool mirrored, GColor color, char slot_mask)
{
    segmented_bar_render(layer, ctx, value, scale, slot_mask, bar_type, available, bar_y, bar_h, 0, mirrored, color);
}
