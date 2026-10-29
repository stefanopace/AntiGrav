#include <pebble.h>
#include "tick.h"
#include "../subscription.h"

#define TIME_MAX_SUBSCRIPTION 5

static SubSlot s_slots[TIME_MAX_SUBSCRIPTION];
static struct tm s_state;
static SubList s_list = {s_slots, TIME_MAX_SUBSCRIPTION, 0, &s_state};
static bool s_subscribed = false;
static int s_seconds_subscribers_count = 0;

static void tick_init(int tag);
static void activate_seconds_if_needed(int tag);
static void tick_deinit();
static void deactivate_seconds_if_unneeded(int tag, const int *remaining, int remaining_count);
static void tick_apply(struct tm *tick_time, TimeUnits time_unit);
static TimeUnits tick_mask(const struct tm *tick_time);
static bool tick_matches_mask(int tag, int mask);

int ag_tick_subscribe(void (*on_change)(const struct tm *tick), TimeUnits update_frequency)
{
    return ag_subscription_subscribe(&s_list, SUB_FN(on_change), (int)update_frequency, tick_init, activate_seconds_if_needed);
}

int ag_tick_unsubscribe(int subscription_handle)
{
    return ag_subscription_unsubscribe(&s_list, subscription_handle, tick_deinit, deactivate_seconds_if_unneeded);
}

static void tick_init(int tag)
{
    time_t now = time(NULL);
    tick_apply(localtime(&now), 0);
    if (tag == (int)SECOND_UNIT)
    {
        return;
    }

    tick_timer_service_subscribe(MINUTE_UNIT, tick_apply);
    s_subscribed = true;
}

static void activate_seconds_if_needed(int tag)
{
    if (tag != (int)SECOND_UNIT)
    {
        return;
    }

    if (s_seconds_subscribers_count == 0)
    {
        time_t now = time(NULL);
        s_state.tm_sec = localtime(&now)->tm_sec;
        tick_timer_service_subscribe(SECOND_UNIT, tick_apply);
        s_subscribed = true;
    }
    s_seconds_subscribers_count++;
}

static void tick_deinit()
{
    if (s_subscribed)
    {
        tick_timer_service_unsubscribe();
        s_subscribed = false;
    }
    s_seconds_subscribers_count = 0;
    s_state = (struct tm){0};
}

static void deactivate_seconds_if_unneeded(int tag, const int *remaining, int remaining_count)
{
    if (tag != (int)SECOND_UNIT)
    {
        return;
    }

    s_seconds_subscribers_count = 0;
    for (int i = 0; i < remaining_count; i++)
    {
        if (remaining[i] == (int)SECOND_UNIT)
        {
            s_seconds_subscribers_count++;
        }
    }

    if (s_seconds_subscribers_count == 0 && remaining_count > 0)
    {
        tick_timer_service_subscribe(MINUTE_UNIT, tick_apply);
    }
}

static void tick_apply(struct tm *tick_time, TimeUnits time_unit)
{
    (void)time_unit;
    TimeUnits mask = tick_mask(tick_time);
    s_state = *tick_time;
    if (mask == 0)
    {
        return;
    }

    ag_subscription_notify_filter(&s_list, tick_matches_mask, (int)mask);
}

static TimeUnits tick_mask(const struct tm *tick_time)
{
    if (s_state.tm_year != tick_time->tm_year)
    {
        return YEAR_UNIT | MONTH_UNIT | DAY_UNIT | HOUR_UNIT | MINUTE_UNIT | SECOND_UNIT;
    }
    if (s_state.tm_mon != tick_time->tm_mon)
    {
        return MONTH_UNIT | DAY_UNIT | HOUR_UNIT | MINUTE_UNIT | SECOND_UNIT;
    }
    if (s_state.tm_mday != tick_time->tm_mday)
    {
        return DAY_UNIT | HOUR_UNIT | MINUTE_UNIT | SECOND_UNIT;
    }
    if (s_state.tm_hour != tick_time->tm_hour)
    {
        return HOUR_UNIT | MINUTE_UNIT | SECOND_UNIT;
    }
    if (s_state.tm_min != tick_time->tm_min)
    {
        return MINUTE_UNIT | SECOND_UNIT;
    }
    if (s_state.tm_sec != tick_time->tm_sec)
    {
        return SECOND_UNIT;
    }

    return 0;
}

static bool tick_matches_mask(int tag, int mask)
{
    return (mask & tag) != 0;
}
