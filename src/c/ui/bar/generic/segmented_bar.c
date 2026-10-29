#include <pebble.h>

#include "segmented_bar.h"
#include "../../layout.h"

static int segments_for_floor(int value, int scale, int total_dots);
static int segments_for_ceil(int value, int scale, int total_dots);

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

    int total_dots = 5;
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
        total_dots = 10;
        ends_right_border = true;
        break;
    case 4:
        starts_left_border = true;
        break;
    case 5:
        jump = true;
        total_dots = 10;
        starts_left_border = true;
        ends_right_border = true;
        break;
    case 6:
        total_dots = 10;
        starts_left_border = true;
        break;
    case 7:
        total_dots = 15;
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
#if PBL_DISPLAY_WIDTH == 144 && PBL_DISPLAY_HEIGHT == 168
            if (total_dots == 5 && (i == 0 || i == 4))
            {
                graphics_fill_rect(ctx, GRect(x, draw_y, DOTS_W + 1, dots_h), BAR_CORNER_RADIUS, corners);
            }
            else if (total_dots == 10 && (i == 0 || i == 1 || i == 8 || i == 9))
            {
                graphics_fill_rect(ctx, GRect(x, draw_y, DOTS_W + 1, dots_h), BAR_CORNER_RADIUS, corners);
            }
            else if (total_dots == 15 && (i == 0 || i == 1 || i == 2 || i == 12 || i == 13 || i == 14))
            {
                graphics_fill_rect(ctx, GRect(x, draw_y, DOTS_W + 1, dots_h), BAR_CORNER_RADIUS, corners);
            }
            else
            {
                graphics_fill_rect(ctx, GRect(x, draw_y, DOTS_W, dots_h), BAR_CORNER_RADIUS, corners);
            }
#else
            graphics_fill_rect(ctx, GRect(x, draw_y, DOTS_W, dots_h), BAR_CORNER_RADIUS, corners);
#endif
        }
        else
        {
            graphics_fill_rect(
                ctx,
                GRect(x + EMPTY_DOTS_GAP_W, empty_dots_y, EMPTY_DOTS_W, empty_h),
                0,
                GCornerNone);
        }

        if (i == 4 && jump)
        {
            x = BAR3_X;
        }
        else
        {
            #if PBL_DISPLAY_WIDTH == 144 && PBL_DISPLAY_HEIGHT == 168
            if (total_dots == 5 && (i == 0 || i == 4))
            {
                x += DOTS_W + 1 + DOTS_GAP;
            }
            else if (total_dots == 10 && (i == 0 || i == 1 || i == 8 || i == 9))
            {
                x += DOTS_W + 1 + DOTS_GAP;
            }
            else if (total_dots == 15 && (i == 0 || i == 1 || i == 2 || i == 12 || i == 13 || i == 14))
            {
                x += DOTS_W + 1 + DOTS_GAP;
            }
            else
            {
                x += DOTS_W + DOTS_GAP;
            }
#else
            x += DOTS_W + DOTS_GAP;
#endif
            
        }
    }
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
