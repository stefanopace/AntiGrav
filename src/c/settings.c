#include <pebble.h>
#include "settings.h"
#include "ui/colors.h"
#include "message_keys.auto.h"
#include "models/message.h"

static int s_message_subscription = -1;

enum
{
    PERSIST_KEY_SETTINGS = 1,
    PERSIST_KEY_VERSION = 2,
};

enum
{
    SETTINGS_VERSION_FONT_ID = 2,
    SETTINGS_VERSION_SECONDS_VISIBILITY = 3,
    SETTINGS_VERSION_COMBO_CONTENT = 4,
    SETTINGS_VERSION_CURRENT = SETTINGS_VERSION_COMBO_CONTENT,
};

typedef struct ClaySettings
{
    uint8_t deprecated_seconds_visibility;
    SettingsLanguage ui_language;
    bool model_hours_leading_zero_enabled;
    bool model_minutes_leading_zero_enabled;
    bool model_seconds_leading_zero_enabled;
    bool model_day_leading_zero_enabled;
    bool model_month_leading_zero_enabled;
    DateFormat ui_date_format;
    DateSeparator ui_date_separator;
    FontId ui_font;
    uint8_t deprecated_legacy_minutes_digit_style;
    BarStyle ui_battery_style;
    BarStyle ui_steps_style;
    int model_seconds_show_on_tap_duration;
    bool ui_bt_icon_enabled;
    bool ui_quiet_time_icon_enabled;
    bool ui_am_pm_icon_enabled;
    VibrationPattern ui_vibration_pattern_on_bt_disconnect;
    bool ui_vibration_on_bt_disconnect_respect_quiet_time;
    bool model_flick_disabled_on_quiet_time;
    int model_low_battery_threshold;
    bool ui_low_battery_icon_enabled;
    bool model_hide_seconds_on_low_battery;
    int model_show_seconds_tap_count;
    int model_step_goal;
    bool ui_goal_icon_enabled;
    SlotContent ui_slot_contents[SLOT_COUNT];
    bool ui_advanced_colors_enabled;
    int ui_palette_override[COLOR_USER_SLOT_COUNT];
    bool ui_phone_low_battery_icon_enabled;
    int model_phone_low_battery_threshold;
    bool ui_phone_charging_icon_enabled;
    SlotContent ui_combo_bar_top;
    SlotContent ui_combo_bar_bottom;
    SlotContent ui_combo_b_top;
    SlotContent ui_combo_b_bottom;
    SlotContent ui_combo_c_top;
    SlotContent ui_combo_c_bottom;
    bool model_hide_seconds_on_quiet_time;
    BarStyle deprecated_slot_bar_style[SLOT_COUNT];
    bool deprecated_slot_bar_mirrored[SLOT_COUNT];
    BarStyle deprecated_combo_top_bar_style[COMBO_BAR_COUNT];
    BarStyle deprecated_combo_bottom_bar_style[COMBO_BAR_COUNT];
    bool deprecated_combo_top_bar_mirrored[COMBO_BAR_COUNT];
    bool deprecated_combo_bottom_bar_mirrored[COMBO_BAR_COUNT];
    BarStyle ui_phone_bar_style;
    BarStyle ui_seconds_bar_style;
    bool ui_battery_mirrored;
    bool ui_phone_bar_mirrored;
    bool ui_steps_mirrored;
    bool ui_seconds_bar_mirrored;
    SlotAlignment ui_icons_alignment;
    SlotAlignment ui_seconds_alignment;
    SlotPlaceholder ui_icons_placeholder;
    SlotPlaceholder ui_seconds_placeholder;
    int ui_battery_color;
    int ui_phone_bar_color;
    int ui_steps_color;
    int ui_seconds_bar_color;
    int ui_placeholder_color[PLACEHOLDER_COLOR_COUNT];
    bool ui_bar_colors_enabled;
    int ui_bar_color;
    bool ui_icon_colors_enabled;
    int ui_icon_color[ICON_COLOR_COUNT];
    bool ui_invert_colors;
    GabbroDateContent ui_gabbro_date_above;
    GabbroDateContent ui_gabbro_date_below;
    PinPopupLarge ui_pin_popup_large;
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

static const SettingName s_language_names[] = {
    {"IT", IT},
    {"EN", EN},
    {"ES", ES},
    {"FR", FR},
    {"PT", PT},
    {"ID", ID},
    {"DE", DE},
    {"NL", NL},
    {"SV", SV},
    {"NB", NB},
    {"TL", TL},
    {"DA", DA},
    {NULL, 0},
};

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

static const SettingName s_vibration_names[] = {
    {"Off", VIBRATION_OFF},
    {"Short", VIBRATION_SHORT},
    {"Long", VIBRATION_LONG},
    {"Double", VIBRATION_DOUBLE},
    {NULL, 0},
};

static const SettingName s_font_names[] = {
    {"Replicant", FONT_REPLICANT},
    {"Racer", FONT_RACER},
    {"Samurai", FONT_SAMURAI},
    {NULL, 0},
};

static const SettingName s_date_format_names[] = {
    {"DayMonthName", DATE_FORMAT_DAY_MONTH_NAME},
    {"DayMonthNum", DATE_FORMAT_DAY_MONTH_NUM},
    {"MonthDayNum", DATE_FORMAT_MONTH_DAY_NUM},
    {"DayMonthYear", DATE_FORMAT_DAY_MONTH_YEAR},
    {"MonthDayYear", DATE_FORMAT_MONTH_DAY_YEAR},
    {"YearMonthDay", DATE_FORMAT_YEAR_MONTH_DAY},
    {"YearDayMonth", DATE_FORMAT_YEAR_DAY_MONTH},
    {"DayNameMonthDay", DATE_FORMAT_DAY_NAME_MONTH_DAY},
    {NULL, 0},
};

static const SettingName s_date_separator_names[] = {
    {"Slash", DATE_SEPARATOR_SLASH},
    {"Dot", DATE_SEPARATOR_DOT},
    {"Hyphen", DATE_SEPARATOR_HYPHEN},
    {NULL, 0},
};

static const SettingName s_gabbro_date_names[] = {
    {"DayName", GABBRO_DATE_DAY_NAME},
    {"MonthName", GABBRO_DATE_MONTH_NAME},
    {"DayMonth", GABBRO_DATE_DAY_MONTH},
    {"MonthDay", GABBRO_DATE_MONTH_DAY},
    {"Day", GABBRO_DATE_DAY},
    {NULL, 0},
};

static const SettingName s_pin_popup_large_names[] = {
    {"Minutes", PIN_POPUP_LARGE_MINUTES},
    {"Hours", PIN_POPUP_LARGE_HOURS},
    {NULL, 0},
};

static const SettingName s_alignment_names[] = {
    {"Automatic", SLOT_ALIGN_AUTOMATIC},
    {"Left", SLOT_ALIGN_LEFT},
    {"Center", SLOT_ALIGN_CENTER},
    {"Right", SLOT_ALIGN_RIGHT},
    {NULL, 0},
};

static const SettingName s_placeholder_names[] = {
    {"Thick", SLOT_PLACEHOLDER_THICK},
    {"None", SLOT_PLACEHOLDER_NONE},
    {"Thin", SLOT_PLACEHOLDER_THIN},
    {NULL, 0},
};

static const SettingName s_combo_names[] = {
    {"WatchBattery", SLOT_CONTENT_BATTERY},
    {"PhoneBattery", SLOT_CONTENT_PHONE_BATTERY},
    {"StepGoal", SLOT_CONTENT_STEPS},
    {"Seconds", SLOT_CONTENT_SECONDS},
    {NULL, 0},
};

static const SettingName s_slot_names[] = {
    {"None", SLOT_CONTENT_NONE},
    {"Icons", SLOT_CONTENT_ICONS},
    {"Battery", SLOT_CONTENT_BATTERY},
    {"ProgressBar", SLOT_CONTENT_BATTERY},
    {"Steps", SLOT_CONTENT_STEPS},
    {"PhoneBattery", SLOT_CONTENT_PHONE_BATTERY},
    {"BatteryCombo", SLOT_CONTENT_COMBO_A},
    {"ComboA", SLOT_CONTENT_COMBO_A},
    {"ComboB", SLOT_CONTENT_COMBO_B},
    {"ComboC", SLOT_CONTENT_COMBO_C},
    {"Seconds", SLOT_CONTENT_SECONDS},
    {"SecondsBar", SLOT_CONTENT_SECONDS_BAR},
    {NULL, 0},
};

static const SettingName s_battery_style_names[] = {
    {"5Bars", FIVE_BARS},
    {"Continuous", CONTINUOUS},
    {NULL, 0},
};

static void default_settings(void);
static int settings_version(void);
static void migrate_persisted_font(int version);
static void migrate_seconds_visibility(int version);
static void migrate_combo_contents(int version);
static void settings_save(void);
static void apply_message(DictionaryIterator *iter);
static void settings_update(DictionaryIterator *iter);
static void assign_bool(DictionaryIterator *iter, const uint32_t *key, bool *dest);
static void assign_name(DictionaryIterator *iter, const uint32_t *key,
                        const SettingName *names, void *dest,
                        void (*assign)(void *dest, uint8_t value));
static void assign_enum(void *dest, uint8_t value);
static void assign_int(void *dest, uint8_t value);
static void assign_positive_int(DictionaryIterator *iter, const uint32_t *key, int *dest);
static void assign_color(DictionaryIterator *iter, const uint32_t *key, int *dest);

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

DateFormat settings_get_date_format()
{
    return settings.ui_date_format;
}

DateSeparator settings_get_date_separator()
{
    return settings.ui_date_separator;
}

GabbroDateContent settings_get_gabbro_date_above()
{
    return settings.ui_gabbro_date_above;
}

GabbroDateContent settings_get_gabbro_date_below()
{
    return settings.ui_gabbro_date_below;
}

bool settings_advanced_colors_enabled()
{
    return settings.ui_advanced_colors_enabled;
}

bool settings_bar_colors_enabled()
{
    return settings.ui_bar_colors_enabled;
}

bool settings_invert_colors_enabled()
{
    return settings.ui_invert_colors;
}

bool settings_icon_colors_enabled()
{
    return settings.ui_icon_colors_enabled;
}

int settings_language()
{
    return settings.ui_language;
}

BarStyle settings_get_bar_style(SlotContent content)
{
    switch (content)
    {
    case SLOT_CONTENT_STEPS:
        return settings.ui_steps_style;
    case SLOT_CONTENT_PHONE_BATTERY:
        return settings.ui_phone_bar_style;
    case SLOT_CONTENT_SECONDS:
        return settings.ui_seconds_bar_style;
    default:
        return settings.ui_battery_style;
    }
}

bool settings_get_bar_mirrored(SlotContent content)
{
    switch (content)
    {
    case SLOT_CONTENT_PHONE_BATTERY:
        return settings.ui_phone_bar_mirrored;
    case SLOT_CONTENT_STEPS:
        return settings.ui_steps_mirrored;
    case SLOT_CONTENT_SECONDS:
        return settings.ui_seconds_bar_mirrored;
    default:
        return settings.ui_battery_mirrored;
    }
}

SlotAlignment settings_get_text_alignment(SlotContent content)
{
    if (content == SLOT_CONTENT_SECONDS)
    {
        return settings.ui_seconds_alignment;
    }
    return settings.ui_icons_alignment;
}

SlotPlaceholder settings_get_text_placeholder(SlotContent content)
{
    if (content == SLOT_CONTENT_SECONDS)
    {
        return settings.ui_seconds_placeholder;
    }
    return settings.ui_icons_placeholder;
}

SlotContent settings_get_combo_bar_top(ComboBarId combo)
{
    if (combo == COMBO_BAR_B)
    {
        return settings.ui_combo_b_top;
    }
    if (combo == COMBO_BAR_C)
    {
        return settings.ui_combo_c_top;
    }
    return settings.ui_combo_bar_top;
}

SlotContent settings_get_combo_bar_bottom(ComboBarId combo)
{
    if (combo == COMBO_BAR_B)
    {
        return settings.ui_combo_b_bottom;
    }
    if (combo == COMBO_BAR_C)
    {
        return settings.ui_combo_c_bottom;
    }
    return settings.ui_combo_bar_bottom;
}

int ag_settings_get_seconds_show_on_tap_duration()
{
    return settings.model_seconds_show_on_tap_duration;
}

bool settings_bt_icon_enabled()
{
    return settings.ui_bt_icon_enabled;
}

bool settings_quiet_time_icon_enabled()
{
    return settings.ui_quiet_time_icon_enabled;
}

bool settings_am_pm_icon_enabled()
{
    return settings.ui_am_pm_icon_enabled;
}

PinPopupLarge settings_get_pin_popup_large()
{
    return settings.ui_pin_popup_large;
}

VibrationPattern settings_vibration_pattern_on_bt_disconnect()
{
    return settings.ui_vibration_pattern_on_bt_disconnect;
}

bool settings_vibration_on_bt_disconnect_respect_quiet_time()
{
    return settings.ui_vibration_on_bt_disconnect_respect_quiet_time;
}

FontId settings_get_font()
{
    return settings.ui_font;
}

bool settings_low_battery_icon_enabled()
{
    return settings.ui_low_battery_icon_enabled;
}

bool settings_phone_low_battery_icon_enabled()
{
    return settings.ui_phone_low_battery_icon_enabled;
}

bool settings_phone_charging_icon_enabled()
{
    return settings.ui_phone_charging_icon_enabled;
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

bool settings_goal_icon_enabled()
{
    return settings.ui_goal_icon_enabled;
}

SlotContent settings_get_slot_content(SlotId slot)
{
    if (slot >= SLOT_COUNT)
    {
        return SLOT_CONTENT_NONE;
    }
    return settings.ui_slot_contents[slot];
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
        if (stored > 0 && stored <= (int)sizeof(settings))
        {
            persist_read_data(PERSIST_KEY_SETTINGS, &settings, stored);
            const int version = settings_version();
            if (version != SETTINGS_VERSION_CURRENT)
            {
                migrate_persisted_font(version);
                migrate_seconds_visibility(version);
                migrate_combo_contents(version);
                settings_save();
            }
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

int settings_get_color_override_hex(int slot)
{
    if (slot < 0 || slot >= COLOR_USER_SLOT_COUNT)
    {
        return 0;
    }
    return settings.ui_palette_override[slot];
}

int settings_get_user_color_hex(int slot)
{
    if (slot < 0 || slot >= COLOR_USER_SLOT_COUNT)
    {
        return 0;
    }

    return settings_get_color_override_hex(slot);
}

int settings_get_bar_color_hex(SlotContent content)
{
    switch (content)
    {
    case SLOT_CONTENT_PHONE_BATTERY:
        return settings.ui_phone_bar_color;
    case SLOT_CONTENT_STEPS:
        return settings.ui_steps_color;
    case SLOT_CONTENT_SECONDS:
        return settings.ui_seconds_bar_color;
    default:
        return settings.ui_battery_color;
    }
}

int settings_get_placeholder_color_hex(int placeholder)
{
    if (placeholder < 0 || placeholder >= PLACEHOLDER_COLOR_COUNT)
    {
        return 0x00FFAA;
    }
    return settings.ui_placeholder_color[placeholder];
}

int settings_get_shared_bar_color_hex()
{
    return settings.ui_bar_color;
}

int settings_get_icon_color_hex(int icon)
{
    if (icon < 0 || icon >= ICON_COLOR_COUNT)
    {
        return 0xFFFFFF;
    }
    return settings.ui_icon_color[icon];
}

static void default_settings(void)
{
    settings.model_hours_leading_zero_enabled = true;
    settings.model_minutes_leading_zero_enabled = true;
    settings.model_seconds_leading_zero_enabled = true;
    settings.model_day_leading_zero_enabled = true;
    settings.model_month_leading_zero_enabled = true;
    settings.ui_date_format = DATE_FORMAT_DAY_MONTH_NAME;
    settings.ui_date_separator = DATE_SEPARATOR_SLASH;
    settings.ui_language = EN;
    settings.model_seconds_show_on_tap_duration = 10;
    settings.ui_font = FONT_REPLICANT;
    settings.ui_battery_style = FIVE_BARS;
    settings.ui_steps_style = CONTINUOUS;
    settings.ui_bt_icon_enabled = true;
    settings.ui_quiet_time_icon_enabled = true;
    settings.ui_am_pm_icon_enabled = false;
    settings.ui_vibration_pattern_on_bt_disconnect = VIBRATION_OFF;
    settings.ui_vibration_on_bt_disconnect_respect_quiet_time = false;
    settings.model_flick_disabled_on_quiet_time = false;
    settings.model_low_battery_threshold = 20;
    settings.ui_low_battery_icon_enabled = true;
    settings.model_phone_low_battery_threshold = 30;
    settings.ui_phone_low_battery_icon_enabled = false;
    settings.ui_phone_charging_icon_enabled = false;
    settings.ui_combo_bar_top = SLOT_CONTENT_BATTERY;
    settings.ui_combo_bar_bottom = SLOT_CONTENT_PHONE_BATTERY;
    settings.ui_combo_b_top = SLOT_CONTENT_SECONDS;
    settings.ui_combo_b_bottom = SLOT_CONTENT_STEPS;
    settings.ui_combo_c_top = SLOT_CONTENT_BATTERY;
    settings.ui_combo_c_bottom = SLOT_CONTENT_SECONDS;
    settings.model_hide_seconds_on_low_battery = false;
    settings.model_show_seconds_tap_count = 1;
    settings.model_step_goal = 10000;
    settings.ui_goal_icon_enabled = true;
    settings.ui_slot_contents[SLOT_LEFT] = SLOT_CONTENT_ICONS;
    settings.ui_slot_contents[SLOT_CENTER] = SLOT_CONTENT_BATTERY;
    settings.ui_slot_contents[SLOT_RIGHT] = SLOT_CONTENT_SECONDS;
    settings.ui_advanced_colors_enabled = false;
    settings.ui_palette_override[COLOR_USER_BACKGROUND] = 0x000000;
    settings.ui_palette_override[COLOR_USER_TEXT] = 0xFFFFFF;
    settings.ui_palette_override[COLOR_USER_ACCENT] = 0x00FFAA;
    settings.ui_palette_override[COLOR_USER_HOURS] = 0xFFFFFF;
    settings.ui_palette_override[COLOR_USER_MINUTES] = 0xFFFFFF;
    settings.ui_palette_override[COLOR_USER_SECONDS] = 0xFFFFFF;
    settings.ui_palette_override[COLOR_USER_ICONS] = 0xFFFFFF;
    settings.ui_palette_override[COLOR_USER_DATE] = 0xFFFFFF;
    settings.ui_palette_override[COLOR_USER_ADVANCED_BACKGROUND] = 0x000000;
    settings.model_hide_seconds_on_quiet_time = false;
    settings.ui_phone_bar_style = CONTINUOUS;
    settings.ui_seconds_bar_style = FIVE_BARS;
    settings.ui_battery_mirrored = false;
    settings.ui_phone_bar_mirrored = false;
    settings.ui_steps_mirrored = false;
    settings.ui_seconds_bar_mirrored = false;
    settings.ui_icons_alignment = SLOT_ALIGN_AUTOMATIC;
    settings.ui_seconds_alignment = SLOT_ALIGN_AUTOMATIC;
    settings.ui_icons_placeholder = SLOT_PLACEHOLDER_THICK;
    settings.ui_seconds_placeholder = SLOT_PLACEHOLDER_THICK;
    settings.ui_battery_color = 0x00FFAA;
    settings.ui_phone_bar_color = 0x00FFAA;
    settings.ui_steps_color = 0x00FFAA;
    settings.ui_seconds_bar_color = 0x00FFAA;
    for (int placeholder = 0; placeholder < PLACEHOLDER_COLOR_COUNT; placeholder++)
    {
        settings.ui_placeholder_color[placeholder] = 0x00FFAA;
    }
    settings.ui_bar_colors_enabled = false;
    settings.ui_bar_color = 0x00FFAA;
    settings.ui_icon_colors_enabled = false;
    for (int icon = 0; icon < ICON_COLOR_COUNT; icon++)
    {
        settings.ui_icon_color[icon] = 0xFFFFFF;
    }
    settings.ui_invert_colors = false;
    settings.ui_gabbro_date_above = GABBRO_DATE_DAY;
    settings.ui_gabbro_date_below = GABBRO_DATE_MONTH_NAME;
#if PBL_DISPLAY_WIDTH == 260 && PBL_DISPLAY_HEIGHT == 260
    settings.ui_pin_popup_large = PIN_POPUP_LARGE_HOURS;
#else
    settings.ui_pin_popup_large = PIN_POPUP_LARGE_MINUTES;
#endif
}

static int settings_version(void)
{
    if (!persist_exists(PERSIST_KEY_VERSION))
    {
        return 0;
    }
    return persist_read_int(PERSIST_KEY_VERSION);
}

static void migrate_persisted_font(int version)
{
    if (version >= SETTINGS_VERSION_FONT_ID)
    {
        return;
    }

    settings.ui_font = FONT_REPLICANT;
    settings.deprecated_legacy_minutes_digit_style = 0;
}

static void migrate_combo_contents(int version)
{
    if (version >= SETTINGS_VERSION_COMBO_CONTENT)
    {
        return;
    }

    static const SlotContent from_combo_byte[] = {
        SLOT_CONTENT_BATTERY,
        SLOT_CONTENT_PHONE_BATTERY,
        SLOT_CONTENT_STEPS,
        SLOT_CONTENT_SECONDS,
    };

    settings.ui_combo_bar_top = from_combo_byte[(uint8_t)settings.ui_combo_bar_top];
    settings.ui_combo_bar_bottom = from_combo_byte[(uint8_t)settings.ui_combo_bar_bottom];
    settings.ui_combo_b_top = from_combo_byte[(uint8_t)settings.ui_combo_b_top];
    settings.ui_combo_b_bottom = from_combo_byte[(uint8_t)settings.ui_combo_b_bottom];
    settings.ui_combo_c_top = from_combo_byte[(uint8_t)settings.ui_combo_c_top];
    settings.ui_combo_c_bottom = from_combo_byte[(uint8_t)settings.ui_combo_c_bottom];
}

static void migrate_seconds_visibility(int version)
{
    if (version >= SETTINGS_VERSION_SECONDS_VISIBILITY)
    {
        return;
    }
    if (settings.deprecated_seconds_visibility != 3)
    {
        settings.model_hide_seconds_on_quiet_time = settings.deprecated_seconds_visibility == 1;
        settings.deprecated_seconds_visibility = 3;
    }
    settings.model_flick_disabled_on_quiet_time = false;
}

static void settings_save(void)
{
    persist_write_data(PERSIST_KEY_SETTINGS, &settings, sizeof(settings));
    persist_write_int(PERSIST_KEY_VERSION, SETTINGS_VERSION_CURRENT);
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

    assign_name(iter, &MESSAGE_KEY_Font, s_font_names, &settings.ui_font, assign_enum);
    assign_name(iter, &MESSAGE_KEY_BatteryStyle, s_battery_style_names, &settings.ui_battery_style, assign_enum);
    assign_name(iter, &MESSAGE_KEY_PhoneBatteryStyle, s_battery_style_names, &settings.ui_phone_bar_style, assign_enum);
    assign_name(iter, &MESSAGE_KEY_StepsStyle, s_battery_style_names, &settings.ui_steps_style, assign_enum);
    assign_name(iter, &MESSAGE_KEY_SecondsBarStyle, s_battery_style_names, &settings.ui_seconds_bar_style, assign_enum);

    assign_name(iter, &MESSAGE_KEY_DateFormat, s_date_format_names, &settings.ui_date_format, assign_enum);
    assign_name(iter, &MESSAGE_KEY_DateSeparator, s_date_separator_names, &settings.ui_date_separator, assign_enum);
    assign_name(iter, &MESSAGE_KEY_GabbroDateAbove, s_gabbro_date_names, &settings.ui_gabbro_date_above, assign_enum);
    assign_name(iter, &MESSAGE_KEY_GabbroDateBelow, s_gabbro_date_names, &settings.ui_gabbro_date_below, assign_enum);

    assign_name(iter, &MESSAGE_KEY_Language, s_language_names, &settings.ui_language, assign_enum);

    assign_name(iter, &MESSAGE_KEY_ShowOnTapDuration, s_tap_duration_names,
                &settings.model_seconds_show_on_tap_duration, assign_int);

    assign_name(iter, &MESSAGE_KEY_ShowSecondsOnTapCount, s_tap_count_names,
                &settings.model_show_seconds_tap_count, assign_int);

    assign_bool(iter, &MESSAGE_KEY_BtIconEnabled, &settings.ui_bt_icon_enabled);
    assign_bool(iter, &MESSAGE_KEY_QuietTimeIconEnabled, &settings.ui_quiet_time_icon_enabled);
    assign_bool(iter, &MESSAGE_KEY_AmPmIconEnabled, &settings.ui_am_pm_icon_enabled);
    assign_name(iter, &MESSAGE_KEY_PinPopupLarge, s_pin_popup_large_names, &settings.ui_pin_popup_large, assign_enum);

    assign_name(iter, &MESSAGE_KEY_BtDisconnectVibe, s_vibration_names,
                &settings.ui_vibration_pattern_on_bt_disconnect, assign_enum);

    assign_bool(iter, &MESSAGE_KEY_NoVibrateOnQuietTime, &settings.ui_vibration_on_bt_disconnect_respect_quiet_time);

    assign_positive_int(iter, &MESSAGE_KEY_LowBatteryThreshold, &settings.model_low_battery_threshold);

    assign_bool(iter, &MESSAGE_KEY_LowBatteryIconEnabled, &settings.ui_low_battery_icon_enabled);

    assign_positive_int(iter, &MESSAGE_KEY_PhoneLowBatteryThreshold, &settings.model_phone_low_battery_threshold);

    assign_bool(iter, &MESSAGE_KEY_PhoneLowBatteryIconEnabled, &settings.ui_phone_low_battery_icon_enabled);
    assign_bool(iter, &MESSAGE_KEY_PhoneChargingIconEnabled, &settings.ui_phone_charging_icon_enabled);
    assign_bool(iter, &MESSAGE_KEY_WatchBatteryMirrored, &settings.ui_battery_mirrored);
    assign_bool(iter, &MESSAGE_KEY_PhoneBatteryMirrored, &settings.ui_phone_bar_mirrored);
    assign_bool(iter, &MESSAGE_KEY_StepGoalMirrored, &settings.ui_steps_mirrored);
    assign_bool(iter, &MESSAGE_KEY_SecondsBarMirrored, &settings.ui_seconds_bar_mirrored);

    assign_name(iter, &MESSAGE_KEY_IconsAlignment, s_alignment_names, &settings.ui_icons_alignment, assign_enum);
    assign_name(iter, &MESSAGE_KEY_SecondsAlignment, s_alignment_names, &settings.ui_seconds_alignment, assign_enum);
    assign_name(iter, &MESSAGE_KEY_IconsPlaceholder, s_placeholder_names, &settings.ui_icons_placeholder, assign_enum);
    assign_name(iter, &MESSAGE_KEY_SecondsPlaceholder, s_placeholder_names, &settings.ui_seconds_placeholder, assign_enum);

    assign_bool(iter, &MESSAGE_KEY_HideSecondsOnLowBattery, &settings.model_hide_seconds_on_low_battery);

    assign_positive_int(iter, &MESSAGE_KEY_StepGoal, &settings.model_step_goal);

    assign_bool(iter, &MESSAGE_KEY_GoalIconEnabled, &settings.ui_goal_icon_enabled);

    assign_name(iter, &MESSAGE_KEY_ComboBarTop, s_combo_names, &settings.ui_combo_bar_top, assign_enum);
    assign_name(iter, &MESSAGE_KEY_ComboBarBottom, s_combo_names, &settings.ui_combo_bar_bottom, assign_enum);
    assign_name(iter, &MESSAGE_KEY_ComboBTop, s_combo_names, &settings.ui_combo_b_top, assign_enum);
    assign_name(iter, &MESSAGE_KEY_ComboBBottom, s_combo_names, &settings.ui_combo_b_bottom, assign_enum);
    assign_name(iter, &MESSAGE_KEY_ComboCTop, s_combo_names, &settings.ui_combo_c_top, assign_enum);
    assign_name(iter, &MESSAGE_KEY_ComboCBottom, s_combo_names, &settings.ui_combo_c_bottom, assign_enum);
    assign_name(iter, &MESSAGE_KEY_SlotLeft, s_slot_names, &settings.ui_slot_contents[SLOT_LEFT], assign_enum);
    assign_name(iter, &MESSAGE_KEY_SlotCenter, s_slot_names, &settings.ui_slot_contents[SLOT_CENTER], assign_enum);
    assign_name(iter, &MESSAGE_KEY_SlotRight, s_slot_names, &settings.ui_slot_contents[SLOT_RIGHT], assign_enum);

    assign_bool(iter, &MESSAGE_KEY_AdvancedColors, &settings.ui_advanced_colors_enabled);

    assign_color(iter, &MESSAGE_KEY_ColorPrimary, &settings.ui_palette_override[COLOR_USER_BACKGROUND]);
    assign_color(iter, &MESSAGE_KEY_ColorSecondary, &settings.ui_palette_override[COLOR_USER_TEXT]);
    assign_color(iter, &MESSAGE_KEY_ColorAccent, &settings.ui_palette_override[COLOR_USER_ACCENT]);
    assign_color(iter, &MESSAGE_KEY_ColorHours, &settings.ui_palette_override[COLOR_USER_HOURS]);
    assign_color(iter, &MESSAGE_KEY_ColorMinutes, &settings.ui_palette_override[COLOR_USER_MINUTES]);
    assign_color(iter, &MESSAGE_KEY_ColorSeconds, &settings.ui_palette_override[COLOR_USER_SECONDS]);
    assign_color(iter, &MESSAGE_KEY_ColorIcons, &settings.ui_palette_override[COLOR_USER_ICONS]);
    assign_color(iter, &MESSAGE_KEY_ColorDate, &settings.ui_palette_override[COLOR_USER_DATE]);
    assign_color(iter, &MESSAGE_KEY_ColorBackground, &settings.ui_palette_override[COLOR_USER_ADVANCED_BACKGROUND]);
    assign_color(iter, &MESSAGE_KEY_ColorBarWatchBattery, &settings.ui_battery_color);
    assign_color(iter, &MESSAGE_KEY_ColorBarPhoneBattery, &settings.ui_phone_bar_color);
    assign_color(iter, &MESSAGE_KEY_ColorBarSteps, &settings.ui_steps_color);
    assign_color(iter, &MESSAGE_KEY_ColorBarSeconds, &settings.ui_seconds_bar_color);
    assign_color(iter, &MESSAGE_KEY_ColorPlaceholderEmpty, &settings.ui_placeholder_color[PLACEHOLDER_EMPTY]);
    assign_color(iter, &MESSAGE_KEY_ColorPlaceholderSeconds, &settings.ui_placeholder_color[PLACEHOLDER_SECONDS]);
    assign_color(iter, &MESSAGE_KEY_ColorPlaceholderIcons, &settings.ui_placeholder_color[PLACEHOLDER_ICONS]);
    assign_color(iter, &MESSAGE_KEY_ColorBar, &settings.ui_bar_color);
    assign_color(iter, &MESSAGE_KEY_ColorIconBluetooth, &settings.ui_icon_color[ICON_COLOR_BLUETOOTH]);
    assign_color(iter, &MESSAGE_KEY_ColorIconQuietTime, &settings.ui_icon_color[ICON_COLOR_QUIET_TIME]);
    assign_color(iter, &MESSAGE_KEY_ColorIconGoal, &settings.ui_icon_color[ICON_COLOR_GOAL]);
    assign_color(iter, &MESSAGE_KEY_ColorIconLowBattery, &settings.ui_icon_color[ICON_COLOR_LOW_BATTERY]);
    assign_color(iter, &MESSAGE_KEY_ColorIconPhoneLowBattery, &settings.ui_icon_color[ICON_COLOR_PHONE_LOW]);
    assign_color(iter, &MESSAGE_KEY_ColorIconPhoneCharging, &settings.ui_icon_color[ICON_COLOR_PHONE_CHARGING]);

    assign_bool(iter, &MESSAGE_KEY_BarColors, &settings.ui_bar_colors_enabled);
    assign_bool(iter, &MESSAGE_KEY_IconColors, &settings.ui_icon_colors_enabled);

    assign_bool(iter, &MESSAGE_KEY_InvertColors, &settings.ui_invert_colors);

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

static void assign_enum(void *dest, uint8_t value)
{
    *(uint8_t *)dest = value;
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

static void assign_color(DictionaryIterator *iter, const uint32_t *key, int *dest)
{
    Tuple *tuple = dict_find(iter, *key);
    if (!tuple)
    {
        return;
    }
    *dest = tuple->value->int32;
}
