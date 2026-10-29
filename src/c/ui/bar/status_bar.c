#include <pebble.h>
#include "generic/continuous_bar.h"
#include "slots.h"
#include "status_bar.h"
#include "../../models/battery.h"
#include "../../models/phone_battery.h"
#include "../../models/bluetooth.h"
#include "../../models/quiet_time.h"
#include "../../models/steps.h"
#include "../../settings.h"
#include "../fonts.h"
#include "../colors.h"

static char s_status_bar_buffer[6];
static int s_battery_level;
static bool s_battery_charging;
static bool s_battery_plugged;
static bool s_battery_low;
static bool s_phone_battery_charging;
static bool s_phone_battery_low;
static bool s_bt_connected;
static bool s_quiet_time_active;
static bool s_goal_reached;

static Layer *s_layer;
static char s_slot_mask;
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
static void draw_status_icons(GContext *ctx, GRect bounds, GTextAlignment align);
static void update_status_buffer(void);
static IconColorId status_icon_color(char glyph);

void status_bar_init(char slot_mask, GRect frame)
{
    s_layer = NULL;
    s_battery_subscription = -1;
    s_phone_battery_subscription = -1;
    s_bluetooth_subscription = -1;
    s_quiet_time_subscription = -1;
    s_steps_subscription = -1;
    if (slot_mask == 0)
    {
        return;
    }

    s_status_bar_buffer[0] = 0;
    s_battery_level = 0;
    s_battery_charging = false;
    s_battery_plugged = false;
    s_battery_low = false;
    s_phone_battery_charging = false;
    s_phone_battery_low = false;
    s_bt_connected = true;
    s_quiet_time_active = false;
    s_goal_reached = false;

    s_slot_mask = slot_mask;
    s_layer = layer_create(frame);
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

static void status_bar_update_proc(Layer *layer, GContext *ctx)
{
    const SlotPlaceholder placeholder = settings_get_text_placeholder(SLOT_CONTENT_ICONS);

    if (s_status_bar_buffer[0] == '\0')
    {
        if (placeholder != SLOT_PLACEHOLDER_NONE)
        {
            continuous_bar_render(layer, ctx, 0, 1, s_slot_mask, placeholder == SLOT_PLACEHOLDER_THICK, false, colors_get_placeholder(PLACEHOLDER_ICONS));
        }
        return;
    }

    if (placeholder != SLOT_PLACEHOLDER_NONE)
    {
        continuous_bar_render(layer, ctx, 0, 1, s_slot_mask, false, false, colors_get_placeholder(PLACEHOLDER_ICONS));
    }

    GRect bounds = GRect(0, 0, SLOTS_SINGLE_W, SLOTS_H);
    GTextAlignment align = GTextAlignmentLeft;
    const SlotAlignment alignment = settings_get_text_alignment(SLOT_CONTENT_ICONS);

    (void)alignment;

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

    GSize icons_size = graphics_text_layout_get_content_size_with_attributes(
        s_status_bar_buffer,
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
            GRect(bounds.origin.x, 0, icons_size.w + PLACEHOLDER_ICON_GAP - TEXT_BG_TRIM, SLOTS_H),
            0,
            GCornerNone);

        break;
    case GTextAlignmentCenter:

        graphics_fill_rect(
            ctx,
            GRect(bounds.origin.x + (bounds.size.w - icons_size.w) / 2 - PLACEHOLDER_ICON_GAP, 0, icons_size.w + PLACEHOLDER_ICON_GAP + PLACEHOLDER_ICON_GAP - TEXT_BG_TRIM, SLOTS_H),
            0,
            GCornerNone);
        break;
    case GTextAlignmentRight:
        graphics_fill_rect(
            ctx,
            GRect(bounds.origin.x + bounds.size.w - icons_size.w - PLACEHOLDER_ICON_GAP, 0, icons_size.w + PLACEHOLDER_ICON_GAP, SLOTS_H),
            0,
            GCornerNone);
        break;
    default:
        break;
    }

    graphics_context_set_antialiased(ctx, false);
    draw_status_icons(ctx, bounds, align);
}

static void show_bluetooth_disconnected_icon(const BluetoothState *state)
{
    s_bt_connected = state->connected;
    update_status_buffer();
}

static void show_quiet_time_icon(const QuietTimeState *state)
{
    s_quiet_time_active = state->active;
    update_status_buffer();
}

static void show_watch_battery_icons(const BatteryState *state)
{
    s_battery_level = state->percent;
    s_battery_charging = state->is_charging;
    s_battery_plugged = state->is_plugged;
    s_battery_low = state->is_low;
    update_status_buffer();
}

static void show_phone_battery_icons(const PhoneBatteryState *state)
{
    s_phone_battery_charging = state->available && state->is_charging;
    s_phone_battery_low = state->available && state->is_low;
    update_status_buffer();
}

static void show_steps_goal_reached_icon(const StepsState *state)
{
    s_goal_reached = state->goal_reached;
    update_status_buffer();
}

static void draw_status_icons(GContext *ctx, GRect bounds, GTextAlignment align)
{
    GFont font = fonts_get_small_font();
    if (!settings_icon_colors_enabled())
    {
        graphics_context_set_text_color(ctx, icons_color_get(ICON_COLOR_COUNT));
        graphics_draw_text(
            ctx,
            s_status_bar_buffer,
            font,
            bounds,
            GTextOverflowModeTrailingEllipsis,
            align,
            NULL);
        return;
    }

    const int len = (int)strlen(s_status_bar_buffer);
    int widths[8];
    int total = 0;
    for (int i = 0; i < len && i < 8; i++)
    {
        char glyph[2] = {s_status_bar_buffer[i], '\0'};
        GSize size = graphics_text_layout_get_content_size_with_attributes(
            glyph,
            font,
            bounds,
            GTextOverflowModeTrailingEllipsis,
            GTextAlignmentLeft,
            NULL);
        widths[i] = size.w;
        total += size.w;
    }

    int x = bounds.origin.x;
    if (align == GTextAlignmentCenter)
    {
        x += (bounds.size.w - total) / 2;
    }
    else if (align == GTextAlignmentRight)
    {
        x += bounds.size.w - total;
    }

    for (int i = 0; i < len && i < 8; i++)
    {
        char glyph[2] = {s_status_bar_buffer[i], '\0'};
        graphics_context_set_text_color(ctx, icons_color_get(status_icon_color(s_status_bar_buffer[i])));
        graphics_draw_text(
            ctx,
            glyph,
            font,
            GRect(x, bounds.origin.y, widths[i], bounds.size.h),
            GTextOverflowModeTrailingEllipsis,
            GTextAlignmentLeft,
            NULL);
        x += widths[i];
    }
}

static void update_status_buffer(void)
{
    int i = 0;

    if (!s_bt_connected)
    {
        s_status_bar_buffer[i++] = 'b';
    }
    if (s_quiet_time_active)
    {
        s_status_bar_buffer[i++] = 'q';
    }
    if (s_battery_charging)
    {
        s_status_bar_buffer[i++] = 'c';
    }
    else if (s_battery_plugged)
    {
        s_status_bar_buffer[i++] = 'f';
    }
    else if (s_battery_low && settings_low_battery_icon_enabled())
    {
        s_status_bar_buffer[i++] = 'l';
    }
    if (s_phone_battery_charging && settings_phone_charging_icon_enabled())
    {
        s_status_bar_buffer[i++] = 'e';
    }
    else if (s_phone_battery_low && settings_phone_low_battery_icon_enabled())
    {
        s_status_bar_buffer[i++] = 'd';
    }
    if (s_goal_reached)
    {
        s_status_bar_buffer[i++] = 'g';
    }

    s_status_bar_buffer[i] = '\0';
    layer_mark_dirty(s_layer);
}

static IconColorId status_icon_color(char glyph)
{
    switch (glyph)
    {
    case 'b':
        return ICON_COLOR_BLUETOOTH;
    case 'q':
        return ICON_COLOR_QUIET_TIME;
    case 'g':
        return ICON_COLOR_GOAL;
    case 'l':
        return ICON_COLOR_LOW_BATTERY;
    case 'd':
        return ICON_COLOR_PHONE_LOW;
    case 'e':
        return ICON_COLOR_PHONE_CHARGING;
    default:
        return ICON_COLOR_COUNT;
    }
}
