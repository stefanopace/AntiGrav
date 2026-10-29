#pragma once
#include <pebble.h>

typedef enum
{
    VIBRATION_OFF = 0,
    VIBRATION_SHORT = 1,
    VIBRATION_LONG = 2,
    VIBRATION_DOUBLE = 3
} VibrationPattern;

typedef enum
{
    CONTINUOUS = 0,
    FIVE_BARS = 1,
} BarStyle;

typedef enum
{
    FONT_REPLICANT = 0,
    FONT_RACER = 1,
    FONT_SAMURAI = 2,
} FontId;

typedef enum
{
    SLOT_CONTENT_NONE = 0,
    SLOT_CONTENT_ICONS = 1,
    SLOT_CONTENT_BATTERY = 2,
    SLOT_CONTENT_SECONDS = 3,
    SLOT_CONTENT_STEPS = 4,
    SLOT_CONTENT_PHONE_BATTERY = 5,
    SLOT_CONTENT_COMBO_A = 6,
    SLOT_CONTENT_SECONDS_BAR = 7,
    SLOT_CONTENT_COMBO_B = 8,
    SLOT_CONTENT_COMBO_C = 9,
} SlotContent;

typedef enum
{
    COMBO_BAR_A = 0,
    COMBO_BAR_B = 1,
    COMBO_BAR_C = 2,
    COMBO_BAR_COUNT = 3,
} ComboBarId;

typedef enum
{
    SLOT_LEFT = 0,
    SLOT_CENTER = 1,
    SLOT_RIGHT = 2,
    SLOT_COUNT = 3,
} SlotId;

typedef enum
{
    SLOT_ALIGN_AUTOMATIC = 0,
    SLOT_ALIGN_LEFT = 1,
    SLOT_ALIGN_CENTER = 2,
    SLOT_ALIGN_RIGHT = 3,
} SlotAlignment;

typedef enum
{
    SLOT_PLACEHOLDER_THICK = 0,
    SLOT_PLACEHOLDER_NONE = 1,
    SLOT_PLACEHOLDER_THIN = 2,
} SlotPlaceholder;

typedef enum
{
    IT = 0,
    EN = 1,
    ES = 2,
    FR = 3,
    PT = 4,
    ID = 5,
    DE = 6,
    NL = 7,
    SV = 8,
    NB = 9,
    TL = 10,
    DA = 11
} SettingsLanguage;

typedef enum
{
    DATE_FORMAT_DAY_MONTH_NAME = 0,
    DATE_FORMAT_DAY_MONTH_NUM = 1,
    DATE_FORMAT_MONTH_DAY_NUM = 2,
    DATE_FORMAT_DAY_MONTH_YEAR = 3,
    DATE_FORMAT_MONTH_DAY_YEAR = 4,
    DATE_FORMAT_YEAR_MONTH_DAY = 5,
    DATE_FORMAT_YEAR_DAY_MONTH = 6,
    DATE_FORMAT_DAY_NAME_MONTH_DAY = 7,
} DateFormat;

typedef enum
{
    DATE_SEPARATOR_SLASH = 0,
    DATE_SEPARATOR_DOT = 1,
    DATE_SEPARATOR_HYPHEN = 2,
} DateSeparator;

typedef enum
{
    GABBRO_DATE_DAY_NAME = 0,
    GABBRO_DATE_MONTH_NAME = 1,
    GABBRO_DATE_DAY_MONTH = 2,
    GABBRO_DATE_MONTH_DAY = 3,
    GABBRO_DATE_DAY = 4,
} GabbroDateContent;

typedef enum
{
    PIN_POPUP_LARGE_MINUTES = 0,
    PIN_POPUP_LARGE_HOURS = 1,
} PinPopupLarge;

void ag_settings_init(void (*reset)(void));
void ag_settings_deinit(void);
bool ag_settings_seconds_disable_on_quiet_time();
bool ag_settings_seconds_reveal_on_flick_enabled();
bool ag_settings_hours_leading_zero_enabled();
bool ag_settings_minutes_leading_zero_enabled();
bool ag_settings_seconds_leading_zero_enabled();
bool ag_settings_day_leading_zero_enabled();
bool ag_settings_month_leading_zero_enabled();
DateFormat settings_get_date_format();
DateSeparator settings_get_date_separator();
GabbroDateContent settings_get_gabbro_date_above();
GabbroDateContent settings_get_gabbro_date_below();
bool settings_advanced_colors_enabled();
bool settings_bar_colors_enabled();
bool settings_icon_colors_enabled();
bool settings_invert_colors_enabled();
int settings_language();
BarStyle settings_get_bar_style(SlotContent content);
bool settings_get_bar_mirrored(SlotContent content);
SlotAlignment settings_get_text_alignment(SlotContent content);
SlotPlaceholder settings_get_text_placeholder(SlotContent content);
int ag_settings_get_seconds_show_on_tap_duration();
bool settings_bt_icon_enabled();
bool settings_quiet_time_icon_enabled();
bool settings_am_pm_icon_enabled();
PinPopupLarge settings_get_pin_popup_large();
VibrationPattern settings_vibration_pattern_on_bt_disconnect();
bool settings_vibration_on_bt_disconnect_respect_quiet_time();
FontId settings_get_font();
bool settings_low_battery_icon_enabled();
bool settings_phone_low_battery_icon_enabled();
bool settings_phone_charging_icon_enabled();
bool ag_settings_hide_seconds_on_low_battery();
int ag_settings_get_show_seconds_tap_count();
bool ag_settings_flick_disabled_on_quiet_time();
int ag_settings_low_battery_threshold();
int ag_settings_phone_low_battery_threshold();
int ag_settings_get_step_goal();
bool settings_goal_icon_enabled();
SlotContent settings_get_combo_bar_top(ComboBarId combo);
SlotContent settings_get_combo_bar_bottom(ComboBarId combo);
SlotContent settings_get_slot_content(SlotId slot);
int settings_get_color_override_hex(int slot);
int settings_get_user_color_hex(int slot);
int settings_get_bar_color_hex(SlotContent content);
int settings_get_placeholder_color_hex(int placeholder);
int settings_get_shared_bar_color_hex();
int settings_get_icon_color_hex(int icon);
