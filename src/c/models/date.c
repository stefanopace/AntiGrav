#include <pebble.h>

#include "../settings.h"
#include "../subscription.h"
#include "date.h"
#include "tick.h"

#define DATE_MAX_SUBSCRIPTION 1

static SubSlot s_slots[DATE_MAX_SUBSCRIPTION];
static DateState s_state;
static SubList s_list = {s_slots, DATE_MAX_SUBSCRIPTION, 0, &s_state};
static int s_tick_subscription = -1;

static void date_init(int tag);
static void date_deinit(void);
static void format_date(const struct tm *tick);
static void write_two_digits(char *buffer, int value, bool leading_zero);
static void write_year(char *buffer, int year);

int ag_date_subscribe(void (*on_change)(const DateState *state))
{
    return ag_subscription_subscribe(&s_list, SUB_FN(on_change), 0, date_init, NULL);
}

int ag_date_unsubscribe(int subscription_handle)
{
    return ag_subscription_unsubscribe(&s_list, subscription_handle, date_deinit, NULL);
}

static void date_init(int tag)
{
    (void)tag;
    s_tick_subscription = ag_tick_subscribe(format_date, DAY_UNIT);
}

static void date_deinit(void)
{
    s_tick_subscription = ag_tick_unsubscribe(s_tick_subscription);
    s_state = (DateState){0};
}

static void format_date(const struct tm *tick)
{
    s_state.weekday = tick->tm_wday;
    s_state.month_index = tick->tm_mon;
    write_two_digits(s_state.day, tick->tm_mday, ag_settings_day_leading_zero_enabled());
    write_two_digits(s_state.month, tick->tm_mon + 1, ag_settings_month_leading_zero_enabled());
    write_year(s_state.year, tick->tm_year + 1900);
    ag_subscription_notify_all(&s_list);
}

static void write_two_digits(char *buffer, int value, bool leading_zero)
{
    int dec = value / 10;
    int uni = value % 10;
    if (dec != 0 || leading_zero)
    {
        buffer[0] = (char)('0' + dec);
        buffer[1] = (char)('0' + uni);
        buffer[2] = '\0';
        return;
    }

    buffer[0] = (char)('0' + uni);
    buffer[1] = '\0';
}

static void write_year(char *buffer, int year)
{
    buffer[0] = (char)('0' + ((year / 1000) % 10));
    buffer[1] = (char)('0' + ((year / 100) % 10));
    buffer[2] = (char)('0' + ((year / 10) % 10));
    buffer[3] = (char)('0' + (year % 10));
    buffer[4] = '\0';
}
