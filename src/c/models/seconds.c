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
static int s_reveal_flick_subscription = -1;
static int s_hide_flick_subscription = -1;
static int s_timeout = 0;
static uint32_t s_used_chain_id = 0;
static bool s_quiet_time_active = false;
static bool s_low_battery = false;

static void seconds_init(int tag);
static void seconds_deinit();
static void deactivate_on_low_battery(const BatteryState *state);
static void deactivate_on_quiet_time(const QuietTimeState *state);
static void reveal_seconds(const FlickState *state);
static void hide_seconds(const FlickState *state);
static bool seconds_blocked();
static void apply_visibility(bool active, int timeout);
static void apply_startup_visibility();
static bool chain_used(const FlickState *state);
static void mark_chain(const FlickState *state);
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
    int hide_count = ag_settings_get_hide_seconds_tap_count();
    if (!(ag_settings_seconds_infinite() && hide_count == 0 && ag_settings_seconds_start_active()))
    {
        s_reveal_flick_subscription = ag_flick_subscribe(reveal_seconds, ag_settings_get_show_seconds_tap_count());
    }
    if (hide_count != 0)
    {
        s_hide_flick_subscription = ag_flick_subscribe(hide_seconds, hide_count);
    }
    apply_startup_visibility();
}

static void seconds_deinit()
{
    s_tick_subscription_handle = ag_tick_unsubscribe(s_tick_subscription_handle);
    s_reveal_flick_subscription = ag_flick_unsubscribe(s_reveal_flick_subscription);
    s_hide_flick_subscription = ag_flick_unsubscribe(s_hide_flick_subscription);
    s_quiet_time_subscription = ag_quiet_time_unsubscribe(s_quiet_time_subscription);
    s_battery_subscription = ag_battery_unsubscribe(s_battery_subscription);
    s_state.amount = 0;
    s_state.active = false;
    s_state.text[0] = '\0';
    s_timeout = 0;
    s_used_chain_id = 0;
    s_quiet_time_active = false;
    s_low_battery = false;
}

static void deactivate_on_low_battery(const BatteryState *state)
{
    if (s_low_battery != state->is_low)
    {
        s_low_battery = state->is_low;
        apply_startup_visibility();
    }
}

static void deactivate_on_quiet_time(const QuietTimeState *state)
{
    s_quiet_time_active = state->active;
    apply_startup_visibility();
}

static void reveal_seconds(const FlickState *state)
{
    if (chain_used(state))
    {
        return;
    }

    if (s_state.active)
    {
        // we reset the timeout timer when a reveal flick is detected while is already active
        if (!ag_settings_seconds_infinite())
        {
            s_timeout = ag_settings_get_seconds_show_on_tap_duration();
        }
        // flick chain is not marked here to permit the same chain to deactivate the seconds
        return;
    }

    apply_visibility(true, ag_settings_get_seconds_show_on_tap_duration());
    if (s_state.active)
    {
        mark_chain(state);
    }
}

static void hide_seconds(const FlickState *state)
{
    if (chain_used(state) || !s_state.active)
    {
        return;
    }

    apply_visibility(false, 0);
    if (!s_state.active)
    {
        mark_chain(state);
    }
}

static bool seconds_blocked()
{
    if (ag_settings_hide_seconds_on_low_battery() && s_low_battery)
    {
        return true;
    }
    return ag_settings_seconds_disable_on_quiet_time() && s_quiet_time_active;
}

static void apply_visibility(bool active, int timeout)
{
    if (seconds_blocked() || !active)
    {
        active = false;
        timeout = 0;
    }
    else if (ag_settings_seconds_infinite())
    {
        timeout = 0;
    }

    if (active && !s_state.active)
    {
        s_timeout = timeout;
        s_state.active = true;
        if (s_tick_subscription_handle == -1)
        {
            s_tick_subscription_handle = ag_tick_subscribe(update_amount, SECOND_UNIT);
        }
    }
    else if (s_state.active && !active)
    {
        s_timeout = 0;
        s_state.active = false;
        s_state.amount = -1;
        s_tick_subscription_handle = ag_tick_unsubscribe(s_tick_subscription_handle);
        write_seconds();
        ag_subscription_notify_all(&s_list);
    }
    else
    {
        s_timeout = timeout;
    }
}

static void apply_startup_visibility()
{
    apply_visibility(ag_settings_seconds_start_active(), ag_settings_get_seconds_show_on_tap_duration());
}

static bool chain_used(const FlickState *state)
{
    return state->chain_id == s_used_chain_id;
}

static void mark_chain(const FlickState *state)
{
    s_used_chain_id = state->chain_id;
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
