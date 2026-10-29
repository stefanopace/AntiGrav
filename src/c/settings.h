#pragma once
#include <pebble.h>

void ag_settings_init(void (*reset)(void));
void ag_settings_deinit(void);
bool ag_settings_seconds_disable_on_quiet_time();
bool ag_settings_seconds_reveal_on_flick_enabled();
bool ag_settings_hours_leading_zero_enabled();
bool ag_settings_minutes_leading_zero_enabled();
bool ag_settings_seconds_leading_zero_enabled();
bool ag_settings_day_leading_zero_enabled();
bool ag_settings_month_leading_zero_enabled();
int ag_settings_get_seconds_show_on_tap_duration();
bool ag_settings_hide_seconds_on_low_battery();
int ag_settings_get_show_seconds_tap_count();
bool ag_settings_flick_disabled_on_quiet_time();
int ag_settings_low_battery_threshold();
int ag_settings_phone_low_battery_threshold();
int ag_settings_get_step_goal();
