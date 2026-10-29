#include <pebble.h>
#include "status_bar.h"
#include "../layout.h"
#include "../../models/battery.h"
#include "../../models/phone_battery.h"
#include "../../models/bluetooth.h"
#include "../../models/quiet_time.h"
#include "../../models/steps.h"
#include "../../settings.h"
#include "../fonts.h"
#include "../colors.h"
#include "../../subscription.h"

typedef enum
{
    STATUS_ICON_BLUETOOTH = 0,
    STATUS_ICON_QUIET_TIME,
    STATUS_ICON_CHARGING,
    STATUS_ICON_PLUGGED,
    STATUS_ICON_LOW_BATTERY,
    STATUS_ICON_PHONE_CHARGING,
    STATUS_ICON_PHONE_LOW,
    STATUS_ICON_GOAL,
    STATUS_ICON_COUNT,
} StatusIcon;

typedef struct
{
    char glyph;
    int height;
    IconColorId color;
} StatusIconInfo;

static const StatusIconInfo s_icon_info[STATUS_ICON_COUNT] = {
    {'b', 16, ICON_COLOR_BLUETOOTH},
    {'q', 19, ICON_COLOR_QUIET_TIME},
    {'c', 26, ICON_COLOR_COUNT},
    {'f', 25, ICON_COLOR_COUNT},
    {'j', 25, ICON_COLOR_LOW_BATTERY},
    {'y', 25, ICON_COLOR_PHONE_CHARGING},
    {'d', 25, ICON_COLOR_PHONE_LOW},
    {'g', 23, ICON_COLOR_GOAL},
};

static StatusIcon s_icons[STATUS_ICON_COUNT];
static int s_icon_count;
static int s_battery_level;
static bool s_battery_charging;
static bool s_battery_plugged;
static bool s_battery_low;
static bool s_phone_battery_charging;
static bool s_phone_battery_low;
static bool s_bt_connected;
static bool s_quiet_time_active;
static bool s_goal_reached;

#define STATUS_BAR_MAX_SUBSCRIPTION 1

static SubSlot s_slots[STATUS_BAR_MAX_SUBSCRIPTION];
static StatusBarState s_state;
static SubList s_list = {s_slots, STATUS_BAR_MAX_SUBSCRIPTION, 0, &s_state};

static Layer *s_layer;
static bool s_icons_visible;
static int s_battery_subscription = -1;
static int s_phone_battery_subscription = -1;
static int s_bluetooth_subscription = -1;
static int s_quiet_time_subscription = -1;
static int s_steps_subscription = -1;

static void status_bar_update_proc(Layer *layer, GContext *ctx);
static void show_bluetooth_disconnected_icon(const BluetoothState *state);
static void show_quiet_time_icon(const QuietTimeState *state);
static void show_watch_battery_icons(const BatteryState *state);
static void show_phone_battery_icons(const PhoneBatteryState *state);
static void show_steps_goal_reached_icon(const StepsState *state);
static void draw_status_icons(GContext *ctx, GRect bounds);
static void update_status_icons(void);
static void push_icon(StatusIcon icon);

void status_bar_init(void)
{
    s_layer = NULL;
    s_battery_subscription = -1;
    s_phone_battery_subscription = -1;
    s_bluetooth_subscription = -1;
    s_quiet_time_subscription = -1;
    s_steps_subscription = -1;

    s_icon_count = 0;
    s_battery_level = 0;
    s_battery_charging = false;
    s_battery_plugged = false;
    s_battery_low = false;
    s_phone_battery_charging = false;
    s_phone_battery_low = false;
    s_bt_connected = true;
    s_quiet_time_active = false;
    s_goal_reached = false;
    s_state.has_icons = false;
    s_icons_visible = false;

    s_layer = layer_create(GRect(STATUS_BAR_X, STATUS_BAR_Y, STATUS_BAR_W, STATUS_BAR_H));
    layer_set_update_proc(s_layer, status_bar_update_proc);
    layer_add_child(layout_parent_layer(), s_layer);

    if (settings_bt_icon_enabled())
    {
        s_bluetooth_subscription = ag_bluetooth_subscribe(show_bluetooth_disconnected_icon);
    }
    if (settings_quiet_time_icon_enabled())
    {
        s_quiet_time_subscription = ag_quiet_time_subscribe(show_quiet_time_icon);
    }
    s_battery_subscription = ag_battery_subscribe(show_watch_battery_icons);
    if (settings_phone_charging_icon_enabled() || settings_phone_low_battery_icon_enabled())
    {
        s_phone_battery_subscription = ag_phone_battery_subscribe(show_phone_battery_icons);
    }
    if (ag_settings_get_step_goal() > 0 && settings_goal_icon_enabled())
    {
        s_steps_subscription = ag_steps_subscribe(show_steps_goal_reached_icon);
    }
}

void status_bar_deinit(void)
{
    s_bluetooth_subscription = ag_bluetooth_unsubscribe(s_bluetooth_subscription);
    s_quiet_time_subscription = ag_quiet_time_unsubscribe(s_quiet_time_subscription);
    s_battery_subscription = ag_battery_unsubscribe(s_battery_subscription);
    s_phone_battery_subscription = ag_phone_battery_unsubscribe(s_phone_battery_subscription);
    s_steps_subscription = ag_steps_unsubscribe(s_steps_subscription);
    if (s_layer)
    {
        layer_destroy(s_layer);
        s_layer = NULL;
    }
}

void status_bar_set_icons_visible(bool visible)
{
    if (s_icons_visible == visible)
    {
        return;
    }
    s_icons_visible = visible;
    if (s_layer)
    {
        layer_mark_dirty(s_layer);
    }
}

int status_bar_subscribe(void (*on_change)(const StatusBarState *state))
{
    return ag_subscription_subscribe(&s_list, SUB_FN(on_change), 0, NULL, NULL);
}

int status_bar_unsubscribe(int subscription_handle)
{
    return ag_subscription_unsubscribe(&s_list, subscription_handle, NULL, NULL);
}

static void push_icon(StatusIcon icon)
{
    if (s_icon_count >= STATUS_ICON_COUNT)
    {
        return;
    }
    s_icons[s_icon_count++] = icon;
}

static void status_bar_update_proc(Layer *layer, GContext *ctx)
{
    if (!s_icons_visible || s_icon_count == 0)
    {
        return;
    }

    draw_status_icons(ctx, layer_get_bounds(layer));
}

static void show_bluetooth_disconnected_icon(const BluetoothState *state)
{
    s_bt_connected = state->connected;
    update_status_icons();
}

static void show_quiet_time_icon(const QuietTimeState *state)
{
    s_quiet_time_active = state->active;
    update_status_icons();
}

static void show_watch_battery_icons(const BatteryState *state)
{
    s_battery_level = state->percent;
    s_battery_charging = state->is_charging;
    s_battery_plugged = state->is_plugged;
    s_battery_low = state->is_low;
    update_status_icons();
}

static void show_phone_battery_icons(const PhoneBatteryState *state)
{
    s_phone_battery_charging = state->available && state->is_charging;
    s_phone_battery_low = state->available && state->is_low;
    update_status_icons();
}

static void show_steps_goal_reached_icon(const StepsState *state)
{
    s_goal_reached = state->goal_reached;
    update_status_icons();
}

static void draw_status_icons(GContext *ctx, GRect bounds)
{
    GFont font = fonts_get_date_font();
    graphics_context_set_antialiased(ctx, false);
    int top = bounds.size.h;
    for (int i = 0; i < s_icon_count; i++)
    {
        const StatusIconInfo *icon = &s_icon_info[s_icons[i]];
        top -= icon->height;
        char glyph[2] = {icon->glyph, '\0'};
        graphics_context_set_text_color(ctx, icons_color_get(icon->color));
        graphics_draw_text(
            ctx,
            glyph,
            font,
            GRect(0, top, STATUS_BAR_W, icon->height),
            GTextOverflowModeFill,
            GTextAlignmentLeft,
            NULL);
        top -= STATUS_BAR_ICON_GAP;
    }
}

static void update_status_icons(void)
{
    s_icon_count = 0;

    if (s_quiet_time_active)
    {
        push_icon(STATUS_ICON_QUIET_TIME);
    }
    if (!s_bt_connected)
    {
        push_icon(STATUS_ICON_BLUETOOTH);
    }
    if (s_battery_charging)
    {
        push_icon(STATUS_ICON_CHARGING);
    }
    else if (s_battery_plugged)
    {
        push_icon(STATUS_ICON_PLUGGED);
    }
    else if (s_battery_low && settings_low_battery_icon_enabled())
    {
        push_icon(STATUS_ICON_LOW_BATTERY);
    }
    if (s_phone_battery_charging && settings_phone_charging_icon_enabled())
    {
        push_icon(STATUS_ICON_PHONE_CHARGING);
    }
    else if (s_phone_battery_low && settings_phone_low_battery_icon_enabled())
    {
        push_icon(STATUS_ICON_PHONE_LOW);
    }
    if (s_goal_reached)
    {
        push_icon(STATUS_ICON_GOAL);
    }

    bool has_icons = s_icon_count > 0;
    if (has_icons != s_state.has_icons)
    {
        s_state.has_icons = has_icons;
        ag_subscription_notify_all(&s_list);
    }

    if (s_layer)
    {
        layer_mark_dirty(s_layer);
    }
}
