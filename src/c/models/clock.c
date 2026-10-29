#include <pebble.h>

#include "../settings.h"
#include "../subscription.h"
#include "clock.h"
#include "tick.h"

#define CLOCK_MAX_SUBSCRIPTION 4

static SubSlot s_slots[CLOCK_MAX_SUBSCRIPTION];
static ClockState s_state;
static SubList s_list = {s_slots, CLOCK_MAX_SUBSCRIPTION, 0, &s_state};
static int s_tick_subscription = -1;

static void clock_init(int tag);
static void clock_deinit(void);
static void format_clock(const struct tm *tick);
static void set_digit(int index, int value, bool visible);
static void write_digits(char *buffer, int dec_index, int uni_index);

int ag_clock_subscribe(void (*on_change)(const ClockState *state))
{
    return ag_subscription_subscribe(&s_list, SUB_FN(on_change), 0, clock_init, NULL);
}

int ag_clock_unsubscribe(int subscription_handle)
{
    return ag_subscription_unsubscribe(&s_list, subscription_handle, clock_deinit, NULL);
}

static void clock_init(int tag)
{
    (void)tag;
    s_tick_subscription = ag_tick_subscribe(format_clock, MINUTE_UNIT);
}

static void clock_deinit(void)
{
    s_tick_subscription = ag_tick_unsubscribe(s_tick_subscription);
    s_state = (ClockState){0};
}

static void format_clock(const struct tm *tick)
{
    int hour = tick->tm_hour;
    s_state.is_pm = hour >= 12;
    if (!clock_is_24h_style())
    {
        if (hour == 0)
        {
            hour = 12;
        }
        else if (hour > 12)
        {
            hour -= 12;
        }
    }

    int hour_dec = hour / 10;
    int minute_dec = tick->tm_min / 10;
    set_digit(CLOCK_HOUR_DEC, hour_dec, hour_dec != 0 || ag_settings_hours_leading_zero_enabled());
    set_digit(CLOCK_HOUR_UNI, hour % 10, true);
    set_digit(CLOCK_MINUTE_DEC, minute_dec, minute_dec != 0 || ag_settings_minutes_leading_zero_enabled());
    set_digit(CLOCK_MINUTE_UNI, tick->tm_min % 10, true);
    write_digits(s_state.hours, CLOCK_HOUR_DEC, CLOCK_HOUR_UNI);
    write_digits(s_state.minutes, CLOCK_MINUTE_DEC, CLOCK_MINUTE_UNI);
    ag_subscription_notify_all(&s_list);
}

static void set_digit(int index, int value, bool visible)
{
    s_state.digits[index].value = value;
    s_state.digits[index].visible = visible;
}

static void write_digits(char *buffer, int dec_index, int uni_index)
{
    const ClockDigit *dec = &s_state.digits[dec_index];
    const ClockDigit *uni = &s_state.digits[uni_index];
    if (dec->visible)
    {
        buffer[0] = (char)('0' + dec->value);
        buffer[1] = (char)('0' + uni->value);
        buffer[2] = '\0';
        return;
    }
    buffer[0] = (char)('0' + uni->value);
    buffer[1] = '\0';
}
