#include <pebble.h>

#include "../../models/clock.h"
#include "../colors.h"
#include "../fonts.h"
#include "../layout.h"
#include "compact_clock.h"

static TextLayer *s_hours_layer;
static TextLayer *s_minutes_layer;
static int s_clock_subscription = -1;

#if PBL_DISPLAY_WIDTH == 260 && PBL_DISPLAY_HEIGHT == 260



#endif

static TextLayer *create_text_layer(Layer *parent, GRect frame, GColor color, GTextAlignment alignment);
static GRect hours_frame(void);
static GTextAlignment hours_alignment(void);
static GRect minutes_frame(void);
static GTextAlignment minutes_alignment(void);
static void set_clock_text(const ClockState *state);

void compact_clock_init(Layer *hours_parent, Layer *minutes_parent)
{
    s_hours_layer = create_text_layer(hours_parent, hours_frame(), colors_get(COLOR_CLOCK_HOURS), hours_alignment());
    s_minutes_layer = create_text_layer(minutes_parent, minutes_frame(), colors_get(COLOR_CLOCK_MINUTES), minutes_alignment());
    s_clock_subscription = ag_clock_subscribe(set_clock_text);
}

void compact_clock_deinit(void)
{
    s_clock_subscription = ag_clock_unsubscribe(s_clock_subscription);
    text_layer_destroy(s_hours_layer);
    s_hours_layer = NULL;
    text_layer_destroy(s_minutes_layer);
    s_minutes_layer = NULL;
}

void compact_clock_set_visible(bool hours, bool minutes)
{
    layer_set_hidden(text_layer_get_layer(s_hours_layer), !hours);
    layer_set_hidden(text_layer_get_layer(s_minutes_layer), !minutes);
}

void compact_clock_shift_hours(int16_t shift_x, int16_t shift_y)
{
    GRect frame = hours_frame();
    frame.origin.x = (int16_t)(frame.origin.x + shift_x);
    frame.origin.y = (int16_t)(frame.origin.y + shift_y);
    layer_set_frame(text_layer_get_layer(s_hours_layer), frame);
}

void compact_clock_shift_minutes(int16_t shift_x)
{
    GRect frame = minutes_frame();
    frame.origin.x = (int16_t)(frame.origin.x + shift_x);
    layer_set_frame(text_layer_get_layer(s_minutes_layer), frame);
}

static TextLayer *create_text_layer(Layer *parent, GRect frame, GColor color, GTextAlignment alignment)
{
    TextLayer *layer = text_layer_create(frame);
    text_layer_set_background_color(layer, GColorClear);
    text_layer_set_text_alignment(layer, alignment);
    text_layer_set_text_color(layer, color);
    text_layer_set_font(layer, fonts_get_small_font());
    layer_set_hidden(text_layer_get_layer(layer), true);
    layer_add_child(parent, text_layer_get_layer(layer));
    return layer;
}

static GRect hours_frame(void)
{
#if PBL_DISPLAY_WIDTH == 260 && PBL_DISPLAY_HEIGHT == 260
    return GRect(CLOCK_HOUR_DEC_X, CLOCK_HOUR_DEC_Y + DIGIT_HEIGHT - DATE_H, CLOCK_COMPACT_HOUR_W, CLOCK_COMPACT_HOUR_H);
#else
    return GRect(CLOCK_COMPACT_HOUR_X, CLOCK_COMPACT_HOUR_Y, CLOCK_COMPACT_HOUR_W, CLOCK_COMPACT_HOUR_H);
#endif
}

static GTextAlignment hours_alignment(void)
{
    return GTextAlignmentRight;
}

static GRect minutes_frame(void)
{
    return GRect(CLOCK_COMPACT_HOUR_X, CLOCK_COMPACT_MINUTE_Y, CLOCK_COMPACT_HOUR_W, CLOCK_COMPACT_HOUR_H);
}

static GTextAlignment minutes_alignment(void)
{
#if PBL_DISPLAY_WIDTH == 260 && PBL_DISPLAY_HEIGHT == 260
    return GTextAlignmentLeft;
#else
    return GTextAlignmentRight;
#endif
}

static void set_clock_text(const ClockState *state)
{
    text_layer_set_text(s_hours_layer, state->hours);
    text_layer_set_text(s_minutes_layer, state->minutes);
}
