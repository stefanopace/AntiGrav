#include <pebble.h>

#include "../../models/unobstructed.h"
#include "../../settings.h"
#include "../layout.h"
#include "big_clock.h"
#include "clock_section.h"
#include "compact_clock.h"
#include "pm_marker.h"

static int s_unobstructed_subscription = -1;

static void rearrange_layers(const UnobstructedState *state);

void clock_section_init()
{
    Layer *parent = layout_parent_layer();
    Layer *fixed = ag_unobstructed_fixed_layer();
    big_clock_init(fixed, parent);
    compact_clock_init(parent, parent);
    pm_marker_init(fixed);
    s_unobstructed_subscription = ag_unobstructed_subscribe(rearrange_layers);
}

void clock_section_deinit()
{
    s_unobstructed_subscription = ag_unobstructed_unsubscribe(s_unobstructed_subscription);
    pm_marker_deinit();
    compact_clock_deinit();
    big_clock_deinit();
}

static void rearrange_layers(const UnobstructedState *state)
{
    const bool popup = state->shift_y != 0;
#if PBL_DISPLAY_WIDTH == 260 && PBL_DISPLAY_HEIGHT == 260
    const bool hours_large = !popup || settings_get_pin_popup_large() == PIN_POPUP_LARGE_HOURS;
    const bool minutes_large = !popup || settings_get_pin_popup_large() == PIN_POPUP_LARGE_MINUTES;
    big_clock_set_visible(hours_large, minutes_large);
    compact_clock_set_visible(!hours_large, !minutes_large);
    pm_marker_set_hours_big(hours_large);
    const int16_t shift_left = (int16_t)((state->shift_y / 9) - 2);
    if (popup && minutes_large)
    {
        const int16_t shift_right = (int16_t)(-shift_left);
        const int16_t shift_up = (int16_t)(state->shift_y - 6);
        big_clock_shift_hours(0);
        big_clock_shift_minutes(shift_right, shift_up);
        compact_clock_shift_hours(shift_left * 3 - 2, 0);
        compact_clock_shift_minutes(0);
        pm_marker_shift(0, shift_up / 4 - 3);
    }
    else
    {
        big_clock_shift_hours(shift_left);
        big_clock_shift_minutes(0, 0);
        compact_clock_shift_hours(0, 0);
        compact_clock_shift_minutes(shift_left);
        pm_marker_shift(shift_left, 0);
    }
#else
    const bool hours_large = !popup || settings_get_pin_popup_large() == PIN_POPUP_LARGE_HOURS;
    const bool minutes_large = !popup || settings_get_pin_popup_large() == PIN_POPUP_LARGE_MINUTES;
    big_clock_set_visible(hours_large, minutes_large);
    compact_clock_set_visible(!hours_large, !minutes_large);
    pm_marker_set_hours_big(hours_large);
#endif
}
