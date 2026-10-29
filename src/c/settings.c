#include <pebble.h>
#include "settings.h"
#include "message_keys.auto.h"
#include "models/message.h"

static int s_message_subscription = -1;

enum
{
    PERSIST_KEY_SETTINGS = 1,
};

typedef struct ClaySettings
{
    bool model_hours_leading_zero_enabled;
    bool model_minutes_leading_zero_enabled;
    bool model_seconds_leading_zero_enabled;
    bool model_day_leading_zero_enabled;
    bool model_month_leading_zero_enabled;
    int model_seconds_show_on_tap_duration;
    bool model_flick_disabled_on_quiet_time;
    int model_low_battery_threshold;
    bool model_hide_seconds_on_low_battery;
    int model_show_seconds_tap_count;
    int model_step_goal;
    int model_phone_low_battery_threshold;
    bool model_hide_seconds_on_quiet_time;
} ClaySettings;

_Static_assert(sizeof(ClaySettings) <= PERSIST_DATA_MAX_LENGTH, "ClaySettings exceeds persist limit");

static ClaySettings settings;

static void (*s_reset)(void);
static AppTimer *s_reset_timer = NULL;

static void run_scheduled_reset(void *data)
{
    (void)data;
    s_reset_timer = NULL;
    s_reset();
}

static void schedule_reset(void)
{
    if (s_reset_timer != NULL)
    {
        return;
    }

    s_reset_timer = app_timer_register(0, run_scheduled_reset, NULL);
}

typedef struct
{
    const char *name;
    uint8_t value;
} SettingName;

static const SettingName s_tap_duration_names[] = {
    {"Disabled", 0},
    {"5", 5},
    {"10", 10},
    {"30", 30},
    {"60", 60},
    {"120", 120},
    {NULL, 0},
};

static const SettingName s_tap_count_names[] = {
    {"0", 0},
    {"1", 1},
    {"2", 2},
    {"3", 3},
    {"4", 4},
    {NULL, 0},
};

static void default_settings(void);
static void settings_save(void);
static void apply_message(DictionaryIterator *iter);
static void settings_update(DictionaryIterator *iter);
static void assign_bool(DictionaryIterator *iter, const uint32_t *key, bool *dest);
static void assign_name(DictionaryIterator *iter, const uint32_t *key,
                        const SettingName *names, void *dest,
                        void (*assign)(void *dest, uint8_t value));
static void assign_int(void *dest, uint8_t value);
static void assign_positive_int(DictionaryIterator *iter, const uint32_t *key, int *dest);

bool ag_settings_seconds_disable_on_quiet_time()
{
    return settings.model_hide_seconds_on_quiet_time;
}

bool ag_settings_seconds_reveal_on_flick_enabled()
{
    return settings.model_show_seconds_tap_count != 0;
}

bool ag_settings_hours_leading_zero_enabled()
{
    return settings.model_hours_leading_zero_enabled;
}

bool ag_settings_minutes_leading_zero_enabled()
{
    return settings.model_minutes_leading_zero_enabled;
}

bool ag_settings_seconds_leading_zero_enabled()
{
    return settings.model_seconds_leading_zero_enabled;
}

bool ag_settings_day_leading_zero_enabled()
{
    return settings.model_day_leading_zero_enabled;
}

bool ag_settings_month_leading_zero_enabled()
{
    return settings.model_month_leading_zero_enabled;
}

int ag_settings_get_seconds_show_on_tap_duration()
{
    return settings.model_seconds_show_on_tap_duration;
}

bool ag_settings_hide_seconds_on_low_battery()
{
    return settings.model_hide_seconds_on_low_battery;
}

bool ag_settings_flick_disabled_on_quiet_time()
{
    return settings.model_flick_disabled_on_quiet_time;
}

int ag_settings_low_battery_threshold()
{
    return settings.model_low_battery_threshold;
}

int ag_settings_phone_low_battery_threshold()
{
    return settings.model_phone_low_battery_threshold;
}

int ag_settings_get_step_goal()
{
    return settings.model_step_goal;
}

int ag_settings_get_show_seconds_tap_count()
{
    return settings.model_show_seconds_tap_count;
}

void ag_settings_init(void (*reset)(void))
{
    s_reset = reset;
    default_settings();
    if (persist_exists(PERSIST_KEY_SETTINGS))
    {
        const int stored = persist_get_size(PERSIST_KEY_SETTINGS);
        if (stored == (int)sizeof(settings))
        {
            persist_read_data(PERSIST_KEY_SETTINGS, &settings, stored);
        }
    }

    s_message_subscription = ag_message_subscribe(apply_message);
}

void ag_settings_deinit(void)
{
    if (s_reset_timer != NULL)
    {
        app_timer_cancel(s_reset_timer);
        s_reset_timer = NULL;
    }

    s_message_subscription = ag_message_unsubscribe(s_message_subscription);
}

static void default_settings(void)
{
    settings.model_hours_leading_zero_enabled = true;
    settings.model_minutes_leading_zero_enabled = true;
    settings.model_seconds_leading_zero_enabled = true;
    settings.model_day_leading_zero_enabled = true;
    settings.model_month_leading_zero_enabled = true;
    settings.model_seconds_show_on_tap_duration = 10;
    settings.model_flick_disabled_on_quiet_time = false;
    settings.model_low_battery_threshold = 20;
    settings.model_phone_low_battery_threshold = 30;
    settings.model_hide_seconds_on_low_battery = false;
    settings.model_show_seconds_tap_count = 1;
    settings.model_step_goal = 10000;
    settings.model_hide_seconds_on_quiet_time = false;
}

static void settings_save(void)
{
    persist_write_data(PERSIST_KEY_SETTINGS, &settings, sizeof(settings));
}

static void apply_message(DictionaryIterator *iter)
{
    if (dict_find(iter, MESSAGE_KEY_PhoneBattery) || dict_find(iter, MESSAGE_KEY_PhoneBatteryRequest))
    {
        return;
    }

    settings_update(iter);
    schedule_reset();
}

static void settings_update(DictionaryIterator *iter)
{
    assign_bool(iter, &MESSAGE_KEY_HideSecondsOnQuietTime, &settings.model_hide_seconds_on_quiet_time);
    assign_bool(iter, &MESSAGE_KEY_HoursLeadingZero, &settings.model_hours_leading_zero_enabled);
    assign_bool(iter, &MESSAGE_KEY_MinutesLeadingZero, &settings.model_minutes_leading_zero_enabled);
    assign_bool(iter, &MESSAGE_KEY_SecondsLeadingZero, &settings.model_seconds_leading_zero_enabled);
    assign_bool(iter, &MESSAGE_KEY_DayLeadingZero, &settings.model_day_leading_zero_enabled);
    assign_bool(iter, &MESSAGE_KEY_MonthLeadingZero, &settings.model_month_leading_zero_enabled);

    assign_name(iter, &MESSAGE_KEY_ShowOnTapDuration, s_tap_duration_names,
                &settings.model_seconds_show_on_tap_duration, assign_int);

    assign_name(iter, &MESSAGE_KEY_ShowSecondsOnTapCount, s_tap_count_names,
                &settings.model_show_seconds_tap_count, assign_int);

    assign_positive_int(iter, &MESSAGE_KEY_LowBatteryThreshold, &settings.model_low_battery_threshold);
    assign_positive_int(iter, &MESSAGE_KEY_PhoneLowBatteryThreshold, &settings.model_phone_low_battery_threshold);

    assign_bool(iter, &MESSAGE_KEY_HideSecondsOnLowBattery, &settings.model_hide_seconds_on_low_battery);
    assign_positive_int(iter, &MESSAGE_KEY_StepGoal, &settings.model_step_goal);

    settings_save();
}

static void assign_bool(DictionaryIterator *iter, const uint32_t *key, bool *dest)
{
    Tuple *tuple = dict_find(iter, *key);
    if (tuple)
    {
        *dest = tuple->value->int32 == 1;
    }
}

static void assign_name(DictionaryIterator *iter, const uint32_t *key,
                        const SettingName *names, void *dest,
                        void (*assign)(void *dest, uint8_t value))
{
    Tuple *tuple = dict_find(iter, *key);
    if (!tuple)
    {
        return;
    }
    const char *value = tuple->value->cstring;
    for (int i = 0; names[i].name != NULL; i++)
    {
        if (strcmp(value, names[i].name) == 0)
        {
            assign(dest, names[i].value);
            return;
        }
    }
}

static void assign_int(void *dest, uint8_t value)
{
    *(int *)dest = value;
}

static void assign_positive_int(DictionaryIterator *iter, const uint32_t *key, int *dest)
{
    Tuple *tuple = dict_find(iter, *key);
    if (!tuple)
    {
        return;
    }
    int value = (int)tuple->value->int32;
    if (value < 0)
    {
        value = 0;
    }
    *dest = value;
}
