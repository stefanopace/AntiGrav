#include <pebble.h>

#include "../../colors.h"
#include "../../layout.h"

static void fill_bar_render_at(GContext *ctx, int value, int scale, char slot_mask, bool mirrored, GColor color, int bar_y, int bar_h, int fill_gap)
{
    if (slot_mask == 0)
    {
        return;
    }

    bool jump = false;
    bool starts_left_border = false;
    bool ends_right_border = false;
    int x = BAR1_X;
    int bar_width = BAR_W;

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
        // The gap is intentionally omitted here so the split is easier.
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

    int max_len = bar_width - 2 * fill_gap;
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

    int inner_h = bar_h - 2 * fill_gap;

    graphics_context_set_fill_color(ctx, color);

    if (jump)
    {
        int split_w = bar_width / 2;
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

        graphics_fill_rect(
            ctx,
            GRect(0, bar_y, split_w, bar_h),
            BAR_CORNER_RADIUS,
            GCornersLeft);

        graphics_fill_rect(
            ctx,
            GRect(BAR3_X, bar_y, split_w, bar_h),
            BAR_CORNER_RADIUS,
            GCornersRight);

        graphics_context_set_fill_color(ctx, colors_get(COLOR_CLOCK_BACKGROUND));

        if (inner_h > 0)
        {
            if (!mirrored)
            {
                graphics_fill_rect(
                    ctx,
                    GRect(0 + fill_gap, bar_y + fill_gap, first_half_len, inner_h),
                    0,
                    GCornerNone);

                graphics_fill_rect(
                    ctx,
                    GRect(BAR3_X, bar_y + fill_gap, second_half_len, inner_h),
                    0,
                    GCornerNone);
            }
            else
            {
                graphics_fill_rect(
                    ctx,
                    GRect(BAR3_X + max_split_len - first_half_len, bar_y + fill_gap, first_half_len, inner_h),
                    0,
                    GCornerNone);

                graphics_fill_rect(
                    ctx,
                    GRect(0 + fill_gap + max_split_len - second_half_len, bar_y + fill_gap, second_half_len, inner_h),
                    0,
                    GCornerNone);
            }
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

        graphics_fill_rect(
            ctx,
            GRect(x, bar_y, bar_width, bar_h),
            BAR_CORNER_RADIUS,
            corners);

        graphics_context_set_fill_color(ctx, colors_get(COLOR_CLOCK_BACKGROUND));

        if (inner_h > 0)
        {
            if (!mirrored)
            {
                graphics_fill_rect(
                    ctx,
                    GRect(x + fill_gap, bar_y + fill_gap, actual_len, inner_h),
                    0,
                    GCornerNone);
            }
            else
            {
                graphics_fill_rect(
                    ctx,
                    GRect(x + fill_gap + max_len - actual_len, bar_y + fill_gap, actual_len, inner_h),
                    0,
                    GCornerNone);
            }
        }
    }
}

void fill_bar_render(Layer *layer, GContext *ctx, int value, int scale, char slot_mask, bool available, bool mirrored, GColor color)
{
    int bar_h = BAR_H;
    int bar_y = BAR_Y;
    if (!available)
    {
        value = 0;
        scale = 1;
        bar_h = CONTINUOUS_UNAVAILABLE_H;
        bar_y = CONTINUOUS_UNAVAILABLE_Y;
    }
    fill_bar_render_at(ctx, value, scale, slot_mask, mirrored, color, bar_y, bar_h, FILL_GAP);
}
