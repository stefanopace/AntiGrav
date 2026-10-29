#include <pebble.h>

#include "../colors.h"
#include "../../settings.h"
#include "../../models/phone_battery.h"
#include "../layout.h"
#include "phone_battery_bar.h"
#include "generic/continuous_bar.h"
#include "generic/segmented_bar.h"

static int s_percent;
static bool s_available;
static char s_slot_mask;
static Layer *s_layer;
static int s_phone_battery_subscription = -1;

static void phone_battery_slot_update_proc(Layer *layer, GContext *ctx);
static void update_percent(const PhoneBatteryState *state);

void phone_battery_bar_init(char slot_mask, GRect frame)
{
    s_layer = NULL;
    s_phone_battery_subscription = -1;
    if (slot_mask == 0)
    {
        return;
    }
    s_slot_mask = slot_mask;
    s_percent = 0;
    s_available = false;

    s_layer = layer_create(frame);
    layer_set_update_proc(s_layer, phone_battery_slot_update_proc);
    layer_add_child(layout_parent_layer(), s_layer);

    s_phone_battery_subscription = ag_phone_battery_subscribe(update_percent);
}

void phone_battery_bar_deinit(void)
{
    s_phone_battery_subscription = ag_phone_battery_unsubscribe(s_phone_battery_subscription);
    if (s_layer)
    {
        layer_destroy(s_layer);
        s_layer = NULL;
    }
}

static void phone_battery_slot_update_proc(Layer *layer, GContext *ctx)
{
    const BarStyle style = settings_get_bar_style(SLOT_CONTENT_PHONE_BATTERY);
    const bool mirrored = settings_get_bar_mirrored(SLOT_CONTENT_PHONE_BATTERY);
    const GColor color = colors_get_bar(SLOT_CONTENT_PHONE_BATTERY);
    if (style == CONTINUOUS)
    {
        continuous_bar_render(layer, ctx, s_percent, 100, s_slot_mask, s_available, mirrored, color);
    }
    else
    {
        segmented_bar_render(layer, ctx, s_percent, 100, s_slot_mask, SEGMENTED_BAR_CEIL, s_available, BAR_Y, BAR_H, SEGMENTED_BAR_EMPTY_H, mirrored, color);
    }
}

static void update_percent(const PhoneBatteryState *state)
{
    if (s_percent != state->percent || s_available != state->available)
    {
        s_percent = state->percent;
        s_available = state->available;
        if (s_layer)
        {
            layer_mark_dirty(s_layer);
        }
    }
}
