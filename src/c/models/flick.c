#include "../settings.h"
#include "flick.h"
#include "../subscription.h"
#include "quiet_time.h"

#define FLICK_MAX_SUBSCRIPTION 2

static SubSlot s_slots[FLICK_MAX_SUBSCRIPTION];
static SubList s_list = {s_slots, FLICK_MAX_SUBSCRIPTION, 0, NULL};

static const int DOUBLE_TAP_INTERVAL = 1000;

static int s_max_tap_count = 1;
static FlickState s_state = {0, 0};
static AppTimer *s_multi_tap_timer;
static int s_quiet_time_subscription = -1;

static void flick_init(int tag);
static void raise_max_tap_count_if_needed(int tag);
static void flick_deinit();
static void recompute_max_tap_count(int tag, const int *remaining, int remaining_count);
static void toggle_accel_tap_subscription(const QuietTimeState *state);
static void count_tap(AccelAxisType axis, int32_t direction);
static bool flick_matches_tap_count(int tag, int tap_count);
static void begin_chain();
static void reset_multi_tap();

int ag_flick_subscribe(void (*on_flick)(const FlickState *state), int flick_count)
{
    return ag_subscription_subscribe(&s_list, SUB_FN(on_flick), flick_count, flick_init, raise_max_tap_count_if_needed);
}

int ag_flick_unsubscribe(int subscription_handle)
{
    return ag_subscription_unsubscribe(&s_list, subscription_handle, flick_deinit, recompute_max_tap_count);
}

static void flick_init(int tag)
{
    (void)tag;
    if (ag_settings_flick_disabled_on_quiet_time())
    {
        s_quiet_time_subscription = ag_quiet_time_subscribe(toggle_accel_tap_subscription);
    }
    else
    {
        accel_tap_service_subscribe(count_tap);
    }
}

static void raise_max_tap_count_if_needed(int tag)
{
    if (tag > s_max_tap_count)
    {
        s_max_tap_count = tag;
    }
}

static void flick_deinit()
{
    if (s_multi_tap_timer)
    {
        app_timer_cancel(s_multi_tap_timer);
        s_multi_tap_timer = NULL;
    }
    s_max_tap_count = 1;
    s_state.count = 0;
    s_state.chain_id = 0;
    accel_tap_service_unsubscribe();
    s_quiet_time_subscription = ag_quiet_time_unsubscribe(s_quiet_time_subscription);
}

static void recompute_max_tap_count(int tag, const int *remaining, int remaining_count)
{
    (void)tag;
    s_max_tap_count = 1;
    for (int i = 0; i < remaining_count; i++)
    {
        if (remaining[i] > s_max_tap_count)
        {
            s_max_tap_count = remaining[i];
        }
    }
}

static void toggle_accel_tap_subscription(const QuietTimeState *state)
{
    if (ag_settings_flick_disabled_on_quiet_time() && state->active)
    {
        accel_tap_service_unsubscribe();
    }
    else
    {
        accel_tap_service_subscribe(count_tap);
    }
}

static void count_tap(AccelAxisType axis, int32_t direction)
{
    if (s_multi_tap_timer)
    {
        app_timer_cancel(s_multi_tap_timer);
        s_multi_tap_timer = NULL;
    }

    if (s_state.count == 0)
    {
        begin_chain();
    }

    s_state.count++;

    s_list.state = &s_state;
    ag_subscription_notify_filter(&s_list, flick_matches_tap_count, s_state.count);
    // Subscribe notifies immediately when state is set, so drop it after this flick.
    s_list.state = NULL;

    if (s_state.count >= s_max_tap_count)
    {
        s_state.count = 0;
    }
    else
    {
        s_multi_tap_timer = app_timer_register(DOUBLE_TAP_INTERVAL, reset_multi_tap, NULL);
    }
}

static bool flick_matches_tap_count(int tag, int tap_count)
{
    return tag == tap_count;
}

static void begin_chain()
{
    s_state.chain_id++;
    if (s_state.chain_id == 0)
    {
        s_state.chain_id = 1;
    }
}

static void reset_multi_tap()
{
    s_state.count = 0;
    s_multi_tap_timer = NULL;
}
