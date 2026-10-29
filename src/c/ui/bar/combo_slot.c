#include <pebble.h>

#include "../../models/battery.h"
#include "../../models/phone_battery.h"
#include "../../models/seconds.h"
#include "../../models/steps.h"
#include "../colors.h"
#include "../../settings.h"
#include "../layout.h"
#include "combo_slot.h"
#include "generic/combo_bar.h"
#include "generic/segmented_bar.h"

static int s_watch_percent;
static int s_phone_percent;
static bool s_phone_available;
static int s_steps_amount;
static int s_steps_goal;
static int s_seconds_amount;
static bool s_seconds_available;
static char s_masks[COMBO_BAR_COUNT];
static Layer *s_layer;
static int s_watch_battery_subscription = -1;
static int s_phone_battery_subscription = -1;
static int s_steps_subscription = -1;
static int s_seconds_subscription = -1;

static void combo_slot_update_proc(Layer *layer, GContext *ctx);
static bool combo_slot_uses(SlotContent content);
static void update_watch_percent(const BatteryState *state);
static void update_phone_percent(const PhoneBatteryState *state);
static void update_steps(const StepsState *state);
static void update_seconds(const SecondsState *state);
static void combo_slot_half(SlotContent content, int *value, int *scale, bool *available, BarStyle *style, int *bar_type, bool *mirrored, GColor *color);

void combo_slot_init(char mask_a, char mask_b, char mask_c, GRect frame)
{
    s_layer = NULL;
    s_watch_battery_subscription = -1;
    s_phone_battery_subscription = -1;
    s_steps_subscription = -1;
    s_seconds_subscription = -1;
    s_masks[COMBO_BAR_A] = mask_a;
    s_masks[COMBO_BAR_B] = mask_b;
    s_masks[COMBO_BAR_C] = mask_c;
    if (mask_a == 0 && mask_b == 0 && mask_c == 0)
    {
        return;
    }
    s_watch_percent = 0;
    s_phone_percent = 0;
    s_phone_available = false;
    s_steps_amount = 0;
    s_steps_goal = 0;
    s_seconds_amount = 0;
    s_seconds_available = false;

    s_layer = layer_create(frame);
    layer_set_update_proc(s_layer, combo_slot_update_proc);
    layer_add_child(layout_parent_layer(), s_layer);

    if (combo_slot_uses(SLOT_CONTENT_BATTERY))
    {
        s_watch_battery_subscription = ag_battery_subscribe(update_watch_percent);
    }
    if (combo_slot_uses(SLOT_CONTENT_PHONE_BATTERY))
    {
        s_phone_battery_subscription = ag_phone_battery_subscribe(update_phone_percent);
    }
    if (combo_slot_uses(SLOT_CONTENT_STEPS))
    {
        s_steps_subscription = ag_steps_subscribe(update_steps);
    }
    if (combo_slot_uses(SLOT_CONTENT_SECONDS))
    {
        s_seconds_subscription = ag_seconds_subscribe(update_seconds);
    }
}

void combo_slot_deinit(void)
{
    s_watch_battery_subscription = ag_battery_unsubscribe(s_watch_battery_subscription);
    s_phone_battery_subscription = ag_phone_battery_unsubscribe(s_phone_battery_subscription);
    s_steps_subscription = ag_steps_unsubscribe(s_steps_subscription);
    s_seconds_subscription = ag_seconds_unsubscribe(s_seconds_subscription);
    if (s_layer)
    {
        layer_destroy(s_layer);
        s_layer = NULL;
    }
}

static void combo_slot_update_proc(Layer *layer, GContext *ctx)
{
    int top_value = 0;
    int bottom_value = 0;
    int top_scale = 100;
    int bottom_scale = 100;
    bool top_available = true;
    bool bottom_available = true;
    BarStyle top_style = FIVE_BARS;
    BarStyle bottom_style = FIVE_BARS;
    int top_bar_type = SEGMENTED_BAR_CEIL;
    int bottom_bar_type = SEGMENTED_BAR_CEIL;
    bool top_mirrored = false;
    bool bottom_mirrored = false;
    GColor top_color = GColorWhite;
    GColor bottom_color = GColorWhite;

    for (int combo = 0; combo < COMBO_BAR_COUNT; combo++)
    {
        if (s_masks[combo] == 0)
        {
            continue;
        }
        combo_slot_half(settings_get_combo_bar_top(combo), &top_value, &top_scale, &top_available, &top_style, &top_bar_type, &top_mirrored, &top_color);
        combo_slot_half(settings_get_combo_bar_bottom(combo), &bottom_value, &bottom_scale, &bottom_available, &bottom_style, &bottom_bar_type, &bottom_mirrored, &bottom_color);
        combo_bar_render(layer, ctx, top_value, top_scale, top_available, top_style, top_bar_type, top_mirrored, top_color, bottom_value, bottom_scale, bottom_available, bottom_style, bottom_bar_type, bottom_mirrored, bottom_color, s_masks[combo]);
    }
}

static bool combo_slot_uses(SlotContent content)
{
    for (int combo = 0; combo < COMBO_BAR_COUNT; combo++)
    {
        if (s_masks[combo] == 0)
        {
            continue;
        }
        if (settings_get_combo_bar_top(combo) == content || settings_get_combo_bar_bottom(combo) == content)
        {
            return true;
        }
    }
    return false;
}

static void update_watch_percent(const BatteryState *state)
{
    if (s_watch_percent == state->percent)
    {
        return;
    }
    s_watch_percent = state->percent;
    if (s_layer)
    {
        layer_mark_dirty(s_layer);
    }
}

static void update_phone_percent(const PhoneBatteryState *state)
{
    if (s_phone_percent == state->percent && s_phone_available == state->available)
    {
        return;
    }
    s_phone_percent = state->percent;
    s_phone_available = state->available;
    if (s_layer)
    {
        layer_mark_dirty(s_layer);
    }
}

static void update_steps(const StepsState *state)
{
    if (s_steps_amount == state->steps_amount && s_steps_goal == state->steps_goal)
    {
        return;
    }
    s_steps_amount = state->steps_amount;
    s_steps_goal = state->steps_goal;
    if (s_layer)
    {
        layer_mark_dirty(s_layer);
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

    if (s_seconds_amount == amount && s_seconds_available == available)
    {
        return;
    }
    s_seconds_amount = amount;
    s_seconds_available = available;
    if (s_layer)
    {
        layer_mark_dirty(s_layer);
    }
}

static void combo_slot_half(SlotContent content, int *value, int *scale, bool *available, BarStyle *style, int *bar_type, bool *mirrored, GColor *color)
{
    if (content == SLOT_CONTENT_PHONE_BATTERY)
    {
        *value = s_phone_percent;
        *scale = 100;
        *available = s_phone_available;
        *bar_type = SEGMENTED_BAR_CEIL;
    }
    else if (content == SLOT_CONTENT_STEPS)
    {
        int goal = s_steps_goal;
        int steps = s_steps_amount;
        if (goal <= 0)
        {
            steps = 0;
            goal = 1;
        }
        *value = steps;
        *scale = goal;
        *available = true;
        *bar_type = SEGMENTED_BAR_FLOOR;
    }
    else if (content == SLOT_CONTENT_SECONDS)
    {
        int amount = s_seconds_amount;
        if (s_seconds_available && amount <= 0)
        {
            amount = 1;
        }
        *value = amount;
        *scale = 60;
        *available = s_seconds_available;
        *bar_type = SEGMENTED_BAR_CEIL;
    }
    else
    {
        *value = s_watch_percent;
        *scale = 100;
        *available = true;
        *bar_type = SEGMENTED_BAR_CEIL;
    }

    *style = settings_get_bar_style(content);
    *mirrored = settings_get_bar_mirrored(content);
    *color = colors_get_bar(content);
}
