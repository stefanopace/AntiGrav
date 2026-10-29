#include <pebble.h>

#include "generic/continuous_bar.h"
#include "../../models/seconds.h"
#include "seconds_slot.h"
#include "../../settings.h"
#include "../fonts.h"
#include "../colors.h"
#include "slots.h"

static char s_sec_buffer[5];
static bool s_active;
static Layer *s_layer;
static char s_slot_mask;
static int s_seconds_subscription = -1;

static void seconds_slot_update_proc(Layer *layer, GContext *ctx);
static void update_buffer(const SecondsState *state);
static void pad_narrow_ones(const char *seconds);

void seconds_slot_init(char slot_mask, GRect frame)
{
    s_layer = NULL;
    s_seconds_subscription = -1;
    if (slot_mask == 0)
    {
        return;
    }
    s_sec_buffer[0] = 0;
    s_slot_mask = slot_mask;

    s_layer = layer_create(frame);
    layer_set_update_proc(s_layer, seconds_slot_update_proc);
    layer_add_child(layout_parent_layer(), s_layer);

    s_seconds_subscription = ag_seconds_subscribe(update_buffer);
}

void seconds_slot_deinit(void)
{
    s_seconds_subscription = ag_seconds_unsubscribe(s_seconds_subscription);
    if (s_layer)
    {
        layer_destroy(s_layer);
        s_layer = NULL;
    }
}

static void seconds_slot_update_proc(Layer *layer, GContext *ctx)
{

    const SlotPlaceholder placeholder = settings_get_text_placeholder(SLOT_CONTENT_SECONDS);

    if (!s_active)
    {
        if (placeholder != SLOT_PLACEHOLDER_NONE)
        {
            continuous_bar_render(layer, ctx, 0, 1, s_slot_mask, placeholder == SLOT_PLACEHOLDER_THICK, false, colors_get_placeholder(PLACEHOLDER_SECONDS));
        }

        return;
    }

    if (placeholder != SLOT_PLACEHOLDER_NONE)
    {
        continuous_bar_render(layer, ctx, 0, 1, s_slot_mask, false, false, colors_get_placeholder(PLACEHOLDER_SECONDS));
    }

    GRect bounds = GRect(0, 0, SLOTS_SINGLE_W, SLOTS_H);
    GTextAlignment align = GTextAlignmentLeft;
    const SlotAlignment alignment = settings_get_text_alignment(SLOT_CONTENT_SECONDS);

    switch (s_slot_mask)
    {
    case 0:
        break;
    case 1:
        bounds = GRect(SLOTS_3DX, 0, SLOTS_SINGLE_W + SLOT_TEXT_WIDTH_EXTRA, SLOTS_H);
        align = GTextAlignmentRight;
        break;
    case 2:
        bounds = GRect(SLOTS_2DX, 0, SLOTS_SINGLE_W + SLOT_TEXT_WIDTH_EXTRA, SLOTS_H);
        align = GTextAlignmentCenter;
        break;
    case 3:
        bounds = GRect(SLOTS_2DX, 0, (2 * SLOTS_SINGLE_W) + SLOTS_GAP + SLOT_TEXT_WIDTH_EXTRA, SLOTS_H);
        align = GTextAlignmentRight;
        break;
    case 4:
        break;
    case 5:
        if (alignment == SLOT_ALIGN_RIGHT)
        {
            bounds = GRect(SLOTS_3DX, 0, SLOTS_SINGLE_W + SLOT_TEXT_WIDTH_EXTRA, SLOTS_H);
        }
        break;
    case 6:
        bounds = GRect(0, 0, 2 * SLOTS_SINGLE_W + SLOTS_GAP + SLOT_TEXT_WIDTH_EXTRA, SLOTS_H);
        break;
    case 7:
        bounds = GRect(0, 0, 3 * SLOTS_SINGLE_W + 2 * SLOTS_GAP + SLOT_TEXT_WIDTH_EXTRA, SLOTS_H);
        align = GTextAlignmentCenter;
        break;
    default:
    }

    switch (alignment)
    {
    case SLOT_ALIGN_AUTOMATIC:
        break;
    case SLOT_ALIGN_LEFT:
        align = GTextAlignmentLeft;
        break;
    case SLOT_ALIGN_RIGHT:
        align = GTextAlignmentRight;
        break;
    case SLOT_ALIGN_CENTER:
        align = GTextAlignmentCenter;
        break;
    default:
    }

    GSize seconds_size = graphics_text_layout_get_content_size_with_attributes(
        s_sec_buffer,
        fonts_get_small_font(),
        bounds,
        GTextOverflowModeTrailingEllipsis,
        align,
        NULL);

    graphics_context_set_fill_color(ctx, colors_get(COLOR_CLOCK_BACKGROUND));
    switch (align)
    {
    case GTextAlignmentLeft:
        graphics_fill_rect(
            ctx,
            GRect(bounds.origin.x, 0, seconds_size.w + PLACEHOLDER_SECONDS_GAP - TEXT_BG_TRIM, SLOTS_H),
            0,
            GCornerNone);

        break;
    case GTextAlignmentCenter:

        graphics_fill_rect(
            ctx,
            GRect(bounds.origin.x + (bounds.size.w - seconds_size.w) / 2 - PLACEHOLDER_SECONDS_GAP, 0, seconds_size.w + PLACEHOLDER_SECONDS_GAP + PLACEHOLDER_SECONDS_GAP - TEXT_BG_TRIM, SLOTS_H),
            0,
            GCornerNone);
        break;
    case GTextAlignmentRight:
        graphics_fill_rect(
            ctx,
            GRect(bounds.origin.x + bounds.size.w - seconds_size.w - PLACEHOLDER_SECONDS_GAP, 0, seconds_size.w + PLACEHOLDER_SECONDS_GAP, SLOTS_H),
            0,
            GCornerNone);
        break;
    default:
        break;
    }

    graphics_context_set_antialiased(ctx, false);

    graphics_context_set_text_color(ctx, seconds_color_get());
    graphics_draw_text(
        ctx,
        s_sec_buffer,
        fonts_get_small_font(),
        bounds,
        GTextOverflowModeTrailingEllipsis,
        align,
        NULL);
}

static void update_buffer(const SecondsState *state)
{
    s_active = state->active;
    if (s_active)
    {
        pad_narrow_ones(state->text);
    }
    layer_mark_dirty(s_layer);
}

static void pad_narrow_ones(const char *seconds)
{
    // Replicant's 1 is narrow so spacing is added to align the whole number
    if (settings_get_font() != FONT_REPLICANT)
    {
        snprintf(s_sec_buffer, sizeof(s_sec_buffer), "%s", seconds);
        return;
    }

    const bool two_digits = seconds[0] != '\0' && seconds[1] != '\0';
    const bool pad_tens = two_digits && seconds[0] == '1';
    const bool pad_units = (two_digits ? seconds[1] : seconds[0]) == '1';

    if (pad_tens && pad_units)
    {
        snprintf(s_sec_buffer, sizeof(s_sec_buffer), " %s ", seconds);
    }
    else if (pad_tens)
    {
        snprintf(s_sec_buffer, sizeof(s_sec_buffer), " %s", seconds);
    }
    else if (pad_units)
    {
        snprintf(s_sec_buffer, sizeof(s_sec_buffer), "%s ", seconds);
    }
    else
    {
        snprintf(s_sec_buffer, sizeof(s_sec_buffer), "%s", seconds);
    }
}
