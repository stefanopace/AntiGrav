#include <pebble.h>

#include "../colors.h"
#include "../../settings.h"
#include "../../models/battery.h"
#include "../layout.h"
#include "battery_bar.h"
#include "generic/continuous_bar.h"
#include "generic/fill_bar.h"
#include "generic/inverted_fill_bar.h"
#include "generic/segmented_bar.h"

static int s_percent;
static char s_slot_mask;
static Layer *s_layer;
static int s_battery_subscription = -1;

static void battery_slot_update_proc(Layer *layer, GContext *ctx);
static void update_percent(const BatteryState *state);
static void battery_bar_mask_change(char bits, bool include);

void battery_bar_init(char slot_mask, GRect frame)
{
    s_layer = NULL;
    s_battery_subscription = -1;
    if (slot_mask == 0 && !settings_placeholder_is_selected(SLOT_PLACEHOLDER_BATTERY))
    {
        return;
    }
    s_slot_mask = slot_mask;
    s_percent = 0;

    s_layer = layer_create(frame);
    layer_set_update_proc(s_layer, battery_slot_update_proc);
    layer_add_child(layout_parent_layer(), s_layer);

    s_battery_subscription = ag_battery_subscribe(update_percent);
}

void battery_bar_deinit(void)
{
    s_battery_subscription = ag_battery_unsubscribe(s_battery_subscription);
    if (s_layer)
    {
        layer_destroy(s_layer);
        s_layer = NULL;
    }
}

void battery_bar_mask_include(char bits)
{
    battery_bar_mask_change(bits, true);
}

void battery_bar_mask_exclude(char bits)
{
    battery_bar_mask_change(bits, false);
}

static void battery_bar_mask_change(char bits, bool include)
{
    if (!s_layer)
    {
        return;
    }

    char next = include ? (char)(s_slot_mask | bits) : (char)(s_slot_mask & ~bits);
    if (next == s_slot_mask)
    {
        return;
    }
    s_slot_mask = next;
    layer_mark_dirty(s_layer);
}

static void battery_slot_update_proc(Layer *layer, GContext *ctx)
{
    const BarStyle style = settings_get_bar_style(SLOT_CONTENT_BATTERY);
    const bool mirrored = settings_get_bar_mirrored(SLOT_CONTENT_BATTERY);
    const GColor color = colors_get_bar(SLOT_CONTENT_BATTERY);
    if (style == FILL)
    {
        fill_bar_render(layer, ctx, s_percent, 100, s_slot_mask, true, mirrored, color);
    }
    else if (style == INVERTED_FILL)
    {
        inverted_fill_bar_render(layer, ctx, s_percent, 100, s_slot_mask, true, mirrored, color);
    }
    else if (style == CONTINUOUS)
    {
        continuous_plain_bar_render(layer, ctx, s_percent, 100, s_slot_mask, true, mirrored, color);
    }
    else
    {
        segmented_bar_render(layer, ctx, s_percent, 100, s_slot_mask, SEGMENTED_BAR_CEIL, true, BAR_Y, BAR_H, SEGMENTED_BAR_EMPTY_H, mirrored, color);
    }
}

static void update_percent(const BatteryState *state)
{
    if (s_percent != state->percent)
    {
        s_percent = state->percent;
        if (s_layer)
        {
            layer_mark_dirty(s_layer);
        }
    }
}
