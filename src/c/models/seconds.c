#include <pebble.h>
#include "seconds.h"
#include "battery.h"
#include "quiet_time.h"
#include "tick.h"
#include "flick.h"
#include "../subscription.h"
#include "../settings.h"

#define SECONDS_MAX_SUBSCRIPTION 3

static SubSlot s_slots[SECONDS_MAX_SUBSCRIPTION];
static SecondsState s_state;
static SubList s_list = {s_slots, SECONDS_MAX_SUBSCRIPTION, 0, &s_state};

static int s_tick_subscription_handle = -1;
static int s_battery_subscription = -1;
static int s_quiet_time_subscription = -1;
static int s_flick_subscription = -1;
static int s_timeout = 0;
static bool s_quiet_time_active = false;
static bool s_low_battery = false;

static void seconds_init(int tag);
static void seconds_deinit();
static void deactivate_on_low_battery(const BatteryState *state);
static void deactivate_on_quiet_time(const QuietTimeState *state);
static void reveal_seconds();
static void update_active(int timeout);
static void update_amount(const struct tm *tick);
static void write_seconds();

int ag_seconds_subscribe(void (*on_change)(const SecondsState *state))
{
    return ag_subscription_subscribe(&s_list, SUB_FN(on_change), 0, seconds_init, NULL);
}

int ag_seconds_unsubscribe(int subscription_handle)
{
    return ag_subscription_unsubscribe(&s_list, subscription_handle, seconds_deinit, NULL);
}

static void seconds_init(int tag)
{
    (void)tag;
    if (ag_settings_hide_seconds_on_low_battery())
    {
        s_battery_subscription = ag_battery_subscribe(deactivate_on_low_battery);
    }
    if (ag_settings_seconds_disable_on_quiet_time())
    {
        s_quiet_time_subscription = ag_quiet_time_subscribe(deactivate_on_quiet_time);
    }
    if (ag_settings_seconds_reveal_on_flick_enabled())
    {
        s_flick_subscription = ag_flick_subscribe(reveal_seconds, ag_settings_get_show_seconds_tap_count());
    }
    update_active(0);
}

static void seconds_deinit()
{
    s_tick_subscription_handle = ag_tick_unsubscribe(s_tick_subscription_handle);
    s_flick_subscription = ag_flick_unsubscribe(s_flick_subscription);
    s_quiet_time_subscription = ag_quiet_time_unsubscribe(s_quiet_time_subscription);
    s_battery_subscription = ag_battery_unsubscribe(s_battery_subscription);
    s_state.amount = 0;
    s_state.active = false;
    s_state.text[0] = '\0';
    s_timeout = 0;
    s_quiet_time_active = false;
    s_low_battery = false;
}

static void deactivate_on_low_battery(const BatteryState *state)
{
    if (s_low_battery != state->is_low)
    {
        s_low_battery = state->is_low;
        update_active(s_timeout);
    }
}

static void deactivate_on_quiet_time(const QuietTimeState *state)
{
    s_quiet_time_active = state->active;
    update_active(s_timeout);
}

static void reveal_seconds()
{
    if (ag_settings_seconds_reveal_on_flick_enabled() && ag_settings_get_seconds_show_on_tap_duration() > 0)
    {
        update_active(ag_settings_get_seconds_show_on_tap_duration() + 1);
    }
}

static void update_active(int timeout)
{
    bool next_active_state = true;
    if (ag_settings_hide_seconds_on_low_battery() && s_low_battery)
    {
        next_active_state = false;
        s_timeout = 0;
    }
    else if (ag_settings_seconds_disable_on_quiet_time() && s_quiet_time_active)
    {
        next_active_state = false;
        s_timeout = 0;
    }
    else if (ag_settings_seconds_reveal_on_flick_enabled() && timeout <= 0)
    {
        next_active_state = false;
    }
    else
    {
        s_timeout = timeout;
    }

    if (next_active_state && !s_state.active)
    {
        s_state.active = true;
        if (s_tick_subscription_handle == -1)
        {
            s_tick_subscription_handle = ag_tick_subscribe(update_amount, SECOND_UNIT);
        }
    }
    else if (s_state.active && !next_active_state)
    {
        s_state.active = false;
        s_state.amount = -1;
        s_tick_subscription_handle = ag_tick_unsubscribe(s_tick_subscription_handle);
        write_seconds();
        ag_subscription_notify_all(&s_list);
    }
}

static void update_amount(const struct tm *tick)
{
    if (s_state.amount != tick->tm_sec)
    {
        s_state.amount = tick->tm_sec;
        if (s_timeout > 0)
        {
            s_timeout--;
            if (s_timeout <= 0)
            {
                s_state.active = false;
                s_state.amount = -1;
                s_tick_subscription_handle = ag_tick_unsubscribe(s_tick_subscription_handle);
            }
        }
    }

    write_seconds();
    ag_subscription_notify_all(&s_list);
}

static void write_seconds()
{
    int amount = s_state.amount;
    if (amount < 0)
    {
        s_state.text[0] = '\0';
        return;
    }

    int dec = amount / 10;
    int uni = amount % 10;
    if (dec != 0 || ag_settings_seconds_leading_zero_enabled())
    {
        s_state.text[0] = (char)('0' + dec);
        s_state.text[1] = (char)('0' + uni);
        s_state.text[2] = '\0';
        return;
    }

    s_state.text[0] = (char)('0' + uni);
    s_state.text[1] = '\0';
}
