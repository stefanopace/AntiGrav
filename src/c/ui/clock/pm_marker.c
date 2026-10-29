#include <pebble.h>

#include "../../models/clock.h"
#include "../../settings.h"
#include "../colors.h"
#include "../fonts.h"
#include "../layout.h"
#include "pm_marker.h"

static TextLayer *s_layer;
static char s_text[2];
static bool s_show;
static int s_hour_dec;
static bool s_hour_dec_visible;
static bool s_hours_is_big = true;
static int s_clock_subscription = -1;

static void update_marker(const ClockState *state);
static void apply_marker(void);
static bool marker_over_digit(void);

void pm_marker_init(Layer *parent)
{
    s_text[0] = '\0';
    s_text[1] = '\0';
    s_layer = text_layer_create(GRect(CLOCK_AM_PM_X, CLOCK_AM_PM_Y, CLOCK_AM_PM_W, CLOCK_AM_PM_H));
    text_layer_set_background_color(s_layer, GColorClear);
    text_layer_set_text_alignment(s_layer, GTextAlignmentLeft);
    text_layer_set_text_color(s_layer, colors_get(COLOR_CLOCK_AM_PM));
    text_layer_set_font(s_layer, fonts_get_small_font());
    text_layer_set_text(s_layer, s_text);
    layer_add_child(parent, text_layer_get_layer(s_layer));
    s_clock_subscription = ag_clock_subscribe(update_marker);
}

void pm_marker_deinit(void)
{
    s_clock_subscription = ag_clock_unsubscribe(s_clock_subscription);
    text_layer_destroy(s_layer);
    s_layer = NULL;
}

void pm_marker_set_hours_big(bool is_big)
{
    if (s_hours_is_big == is_big)
    {
        return;
    }
    s_hours_is_big = is_big;
    apply_marker();
}

#if PBL_DISPLAY_WIDTH == 260 && PBL_DISPLAY_HEIGHT == 260
void pm_marker_shift(int16_t shift_x, int16_t shift_y)
{
    GRect frame = GRect(CLOCK_AM_PM_X, CLOCK_AM_PM_Y, CLOCK_AM_PM_W, CLOCK_AM_PM_H);
    frame.origin.x = (int16_t)(frame.origin.x + shift_x);
    frame.origin.y = (int16_t)(frame.origin.y + shift_y);
    layer_set_frame(text_layer_get_layer(s_layer), frame);
}
#endif

static void update_marker(const ClockState *state)
{
    const ClockDigit *tens = &state->digits[CLOCK_HOUR_DEC];
    s_hour_dec = tens->value;
    s_hour_dec_visible = tens->visible;
    s_show = settings_am_pm_icon_enabled() && !clock_is_24h_style() && state->is_pm;
    apply_marker();
}

static void apply_marker(void)
{
    s_text[0] = '\0';
    s_text[1] = '\0';
    if (s_show)
    {
        s_text[0] = 'p';
        text_layer_set_text_color(
            s_layer,
            marker_over_digit() ? colors_get(COLOR_CLOCK_BACKGROUND) : colors_get(COLOR_CLOCK_AM_PM));
    }
    layer_mark_dirty(text_layer_get_layer(s_layer));
}

static bool marker_over_digit(void)
{
#if PBL_DISPLAY_WIDTH == 260 && PBL_DISPLAY_HEIGHT == 260
    return false;
#else
    const bool overlaps_one = settings_get_font() == FONT_RACER && s_hour_dec == 1;
    return s_hours_is_big && s_hour_dec_visible && (s_hour_dec == 0 || s_hour_dec == 2 || overlaps_one);
#endif
}
