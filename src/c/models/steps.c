#include <pebble.h>

#include "tick.h"
#include "../subscription.h"
#include "steps.h"
#include "../settings.h"

#define STEPS_MAX_SUBSCRIPTION 3

static SubSlot s_slots[STEPS_MAX_SUBSCRIPTION];
static StepsState s_state;
static SubList s_list = {s_slots, STEPS_MAX_SUBSCRIPTION, 0, &s_state};
static int s_tick_subscription = -1;
static int s_steps_goal;

static void steps_init(int tag);
static void steps_deinit();
static void steps_detector_refresh(const struct tm *tick);
static int steps_detector_peek_steps_today();

int ag_steps_subscribe(void (*on_change)(const StepsState *state))
{
    return ag_subscription_subscribe(&s_list, SUB_FN(on_change), 0, steps_init, NULL);
}

int ag_steps_unsubscribe(int subscription_handle)
{
    return ag_subscription_unsubscribe(&s_list, subscription_handle, steps_deinit, NULL);
}

static void steps_init(int tag)
{
    (void)tag;
    s_steps_goal = ag_settings_get_step_goal();
    s_state.steps_goal = s_steps_goal;
    s_tick_subscription = ag_tick_subscribe(steps_detector_refresh, MINUTE_UNIT);
}

static void steps_deinit()
{
    s_tick_subscription = ag_tick_unsubscribe(s_tick_subscription);
    s_state.steps_amount = 0;
    s_state.steps_goal = 0;
    s_state.goal_reached = false;
}

static void steps_detector_refresh(const struct tm *tick)
{
    s_state.steps_amount = steps_detector_peek_steps_today();
    s_state.steps_goal = s_steps_goal;
    s_state.goal_reached = (s_steps_goal > 0) && (s_state.steps_amount >= s_steps_goal);

    ag_subscription_notify_all(&s_list);
}

static int steps_detector_peek_steps_today()
{
    return (int)health_service_sum_today(HealthMetricStepCount);
}
