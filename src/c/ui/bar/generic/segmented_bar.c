#include <pebble.h>

#include "segmented_bar.h"
#include "../../layout.h"

static int segments_for_floor(int value, int scale, int total_dots);
static int segments_for_ceil(int value, int scale, int total_dots);
static int segment_width(int index, char slot_mask);

void segmented_bar_render(Layer *layer, GContext *ctx, int value, int scale, char slot_mask, int bar_type, bool available, int bar_y, int bar_h, int empty_h, bool mirrored, GColor color)
{

    int dots_h = bar_h;
    int draw_y = bar_y;
    const int empty_dots_y = bar_y + ((bar_h - empty_h) / 2);
    if (!available)
    {
        value = 1;
        scale = 1;
        dots_h = SEGMENTED_BAR_EMPTY_H;
        draw_y = bar_y + ((bar_h - dots_h) / 2);
    }

    int total_dots = 3;
    bool jump = false;
    bool starts_left_border = false;
    bool ends_right_border = false;
    int x = BAR1_X;

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
        total_dots = 6;
        ends_right_border = true;
        break;
    case 4:
        starts_left_border = true;
        break;
    case 5:
        jump = true;
        total_dots = 6;
        starts_left_border = true;
        ends_right_border = true;
        break;
    case 6:
        total_dots = 6;
        starts_left_border = true;
        break;
    case 7:
        total_dots = 9;
        starts_left_border = true;
        ends_right_border = true;
        break;
    default:
    }

    int active_dots = 0;

    if (bar_type == SEGMENTED_BAR_FLOOR)
    {
        active_dots = segments_for_floor(value, scale, total_dots);
    }
    else
    {
        active_dots = segments_for_ceil(value, scale, total_dots);
    }

    graphics_context_set_fill_color(ctx, color);

    for (int i = 0; i < total_dots; i++)
    {
        const int width = segment_width(i, slot_mask);

        if ((!mirrored && i < active_dots) || (mirrored && i >= (total_dots - active_dots)))
        {
            GCornerMask corners = GCornerNone;

            if (i == 0 && starts_left_border)
            {
                corners = GCornersLeft;
            }
            else if (i == total_dots - 1 && ends_right_border)
            {
                corners = GCornersRight;
            }
            graphics_fill_rect(ctx, GRect(x, draw_y, width, dots_h), BAR_CORNER_RADIUS, corners);
        }
        else
        {
            graphics_fill_rect(
                ctx,
                GRect(x + (width - EMPTY_DOTS_W) / 2, empty_dots_y, EMPTY_DOTS_W, empty_h),
                0,
                GCornerNone);
        }

        if (i == 2 && jump)
        {
            x = BAR3_X;
        }
        else
        {
            x += width + DOTS_GAP;
        }
    }
}

static int segment_width(int index, char slot_mask)
{
    bool wide = false;

    switch (slot_mask)
    {
    case 1:
        wide = index == 2;
        break;
    case 2:
        wide = index == 1;
        break;
    case 3:
        wide = index == 1 || index == 5;
        break;
    case 4:
        wide = index == 0;
        break;
    case 5:
        wide = index == 0 || index == 5;
        break;
    case 6:
        wide = index == 0 || index == 4;
        break;
    case 7:
        wide = index == 0 || index == 4 || index == 8;
        break;
    default:
        break;
    }

    return wide ? DOTS_W + 1 : DOTS_W;
}

static int segments_for_floor(int value, int scale, int total_dots)
{
    if (value <= 0 || scale <= 0)
    {
        return 0;
    }
    if (value >= scale)
    {
        return total_dots;
    }
    return value * total_dots / scale;
}

static int segments_for_ceil(int value, int scale, int total_dots)
{
    if (value <= 0 || scale <= 0)
    {
        return 0;
    }
    if (value >= scale)
    {
        return total_dots;
    }
    int product = value * total_dots;
    int segments = product / scale;
    if (product % scale != 0)
    {
        segments += 1;
    }
    return segments;
}
