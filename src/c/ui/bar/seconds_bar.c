#include <pebble.h>

#include "../colors.h"
#include "../../settings.h"
#include "../../models/seconds.h"
#include "../layout.h"
#include "seconds_bar.h"
#include "slots.h"
#include "generic/continuous_bar.h"
#include "generic/fill_bar.h"
#include "generic/inverted_fill_bar.h"
#include "generic/segmented_bar.h"

static int s_amount;
static bool s_available;
static char s_slot_mask;
static Layer *s_layer;
static int s_seconds_subscription = -1;

static void seconds_bar_update_proc(Layer *layer, GContext *ctx);
static void update_seconds(const SecondsState *state);
static void sync_placeholder_mask(void);

void seconds_bar_init(char slot_mask, GRect frame)
{
    s_layer = NULL;
    s_seconds_subscription = -1;
    if (slot_mask == 0)
    {
        return;
    }
    s_slot_mask = slot_mask;
    s_amount = 0;
    s_available = false;

    s_layer = layer_create(frame);
    layer_set_update_proc(s_layer, seconds_bar_update_proc);
    layer_add_child(layout_parent_layer(), s_layer);

    s_seconds_subscription = ag_seconds_subscribe(update_seconds);
}

void seconds_bar_deinit(void)
{
    s_seconds_subscription = ag_seconds_unsubscribe(s_seconds_subscription);
    if (s_layer)
    {
        layer_destroy(s_layer);
        s_layer = NULL;
    }
}

static void seconds_bar_update_proc(Layer *layer, GContext *ctx)
{
    const SlotPlaceholder placeholder = settings_get_text_placeholder(SLOT_CONTENT_SECONDS_BAR);
    if (!s_available)
    {
        if (!settings_placeholder_is_source(placeholder) && placeholder != SLOT_PLACEHOLDER_NONE)
        {
            continuous_plain_bar_render(layer, ctx, 100, 1, s_slot_mask, placeholder == SLOT_PLACEHOLDER_THICK, false, colors_get_placeholder(PLACEHOLDER_SECONDS));
        }
        return;
    }

    const BarStyle style = settings_get_bar_style(SLOT_CONTENT_SECONDS);
    const bool mirrored = settings_get_bar_mirrored(SLOT_CONTENT_SECONDS);
    const GColor color = colors_get_bar(SLOT_CONTENT_SECONDS);
    if (style == FILL)
    {
        fill_bar_render(layer, ctx, s_amount, 60, s_slot_mask, s_available, mirrored, color);
    }
    else if (style == INVERTED_FILL)
    {
        inverted_fill_bar_render(layer, ctx, s_amount, 60, s_slot_mask, s_available, mirrored, color);
    }
    else if (style == CONTINUOUS)
    {
        continuous_plain_bar_render(layer, ctx, s_amount, 60, s_slot_mask, s_available, mirrored, color);
    }
    else
    {
        int amount = s_amount;
        if (s_available && amount <= 0)
        {
            amount = 1;
        }
        segmented_bar_render(layer, ctx, amount, 60, s_slot_mask, SEGMENTED_BAR_CEIL, s_available, BAR_Y, BAR_H, SEGMENTED_BAR_EMPTY_H, mirrored, color);
    }
}

static void update_seconds(const SecondsState *state)
{
    int amount = state->amount;
    const bool available = state->active && amount >= 0;
    if (available)
    {
        if (amount > 59)
        {
            amount = 59;
        }
    }
    else
    {
        amount = 0;
    }

    const bool changed = s_amount != amount || s_available != available;
    s_amount = amount;
    s_available = available;
    sync_placeholder_mask();
    if (!changed || !s_layer)
    {
        return;
    }
    layer_mark_dirty(s_layer);
}

static void sync_placeholder_mask(void)
{
    const SlotPlaceholder placeholder = settings_get_text_placeholder(SLOT_CONTENT_SECONDS_BAR);
    if (!s_available)
    {
        slots_mask_include(placeholder, s_slot_mask);
        return;
    }
    slots_mask_exclude(placeholder, s_slot_mask);
}
