#include <pebble.h>

#include "tick.h"
#include "../subscription.h"
#include "quiet_time.h"

#define QUIET_TIME_MAX_SUBSCRIPTION 4

static SubSlot s_slots[QUIET_TIME_MAX_SUBSCRIPTION];
static QuietTimeState s_state;
static SubList s_list = {s_slots, QUIET_TIME_MAX_SUBSCRIPTION, 0, &s_state};

static bool s_first_check = true;
static AppTimer *s_app_timer;
static int s_tick_subscription = -1;

static void quiet_time_init(int tag);
static void quiet_time_deinit();
static void schedule_quiet_time_check(const struct tm *tick);
static void handle_quiet_time_switch();

int ag_quiet_time_subscribe(void (*on_change)(const QuietTimeState *state))
{
    return ag_subscription_subscribe(&s_list, SUB_FN(on_change), 0, quiet_time_init, NULL);
}

int ag_quiet_time_unsubscribe(int subscription_handle)
{
    return ag_subscription_unsubscribe(&s_list, subscription_handle, quiet_time_deinit, NULL);
}

static void quiet_time_init(int tag)
{
    (void)tag;
    s_state.active = quiet_time_is_active();

    s_tick_subscription = ag_tick_subscribe(schedule_quiet_time_check, MINUTE_UNIT);
}

static void quiet_time_deinit()
{
    if (s_app_timer)
    {
        app_timer_cancel(s_app_timer);
        s_app_timer = NULL;
    }
    s_tick_subscription = ag_tick_unsubscribe(s_tick_subscription);
    s_first_check = true;
}

static void schedule_quiet_time_check(const struct tm *tick)
{
    if (s_first_check)
    {
        s_first_check = false;
        return;
    }
    // quiet_time_is_active() still reports the previous minute during this tick.
    // A scheduled start or end shows up about a second later.
    s_app_timer = app_timer_register(1000, handle_quiet_time_switch, NULL);
}

static void handle_quiet_time_switch()
{
    s_app_timer = NULL;
    bool active = quiet_time_is_active();
    if (active != s_state.active)
    {
        s_state.active = active;
        ag_subscription_notify_all(&s_list);
    }
}
