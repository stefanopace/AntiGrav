#include <pebble.h>

#include "../../models/clock.h"
#include "../../settings.h"
#include "../colors.h"
#include "../fonts.h"
#include "../layout.h"
#include "pm_marker.h"

static TextLayer *s_layer;
static GRect s_base;
static char s_text[2];
static bool s_show;
static int s_clock_subscription = -1;

static void update_marker(const ClockState *state);
static void apply_marker(void);

void pm_marker_init(Layer *parent)
{
    s_text[0] = '\0';
    s_text[1] = '\0';
    s_base = GRect(CLOCK_AM_PM_X, CLOCK_AM_PM_Y, CLOCK_AM_PM_W, CLOCK_AM_PM_H);
    s_layer = text_layer_create(s_base);
    text_layer_set_background_color(s_layer, GColorClear);
    text_layer_set_text_alignment(s_layer, GTextAlignmentLeft);
    text_layer_set_text_color(s_layer, colors_get(COLOR_CLOCK_AM_PM));
    text_layer_set_font(s_layer, fonts_get_date_font());
    text_layer_set_text(s_layer, s_text);
    layer_add_child(parent, text_layer_get_layer(s_layer));
    s_clock_subscription = ag_clock_subscribe(update_marker);
}

void pm_marker_set_offset_x(int16_t offset)
{
    GRect frame = s_base;
    frame.origin.x = (int16_t)(s_base.origin.x + offset);
    layer_set_frame(text_layer_get_layer(s_layer), frame);
}

void pm_marker_deinit(void)
{
    s_clock_subscription = ag_clock_unsubscribe(s_clock_subscription);
    text_layer_destroy(s_layer);
    s_layer = NULL;
}

static void update_marker(const ClockState *state)
{
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
    }
    text_layer_set_text_color(s_layer, colors_get(COLOR_CLOCK_AM_PM));
    layer_mark_dirty(text_layer_get_layer(s_layer));
}
