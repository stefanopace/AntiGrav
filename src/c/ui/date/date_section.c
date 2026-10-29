#include <pebble.h>
#include "date_section.h"

#include "../../models/date.h"
#include "../../models/unobstructed.h"
#include "date_text.h"
#include "../../settings.h"
#include "../fonts.h"
#include "../colors.h"
#include "../layout.h"

#define DATE_BUFFER_SIZE 8

static Layer *s_date_layer;
static char s_left_buffer[DATE_BUFFER_SIZE];
static char s_right_buffer[DATE_BUFFER_SIZE];
static char s_center_buffer[DATE_BUFFER_SIZE];
static int s_date_subscription = -1;

static GRect date_section_frame(void);
static void date_layer_update_proc(Layer *layer, GContext *ctx);
static void refresh_date(const DateState *state);
static void fill_buffers(const DateState *state);
static bool format_is_split(DateFormat format);
static void draw_date_text(GContext *ctx, const char *text, GRect bounds, GTextAlignment alignment);

void date_section_init()
{
    s_left_buffer[0] = '\0';
    s_right_buffer[0] = '\0';
    s_center_buffer[0] = '\0';

    s_date_layer = layer_create(date_section_frame());
    layer_set_update_proc(s_date_layer, date_layer_update_proc);
    layer_add_child(ag_unobstructed_shifting_layer(), s_date_layer);

    s_date_subscription = ag_date_subscribe(refresh_date);
}

void date_section_deinit()
{
    s_date_subscription = ag_date_unsubscribe(s_date_subscription);
    layer_destroy(s_date_layer);
    s_date_layer = NULL;
}

static GRect date_section_frame(void)
{
    int x = DATE_LEFT_X;
    int width = DATE_DISPLAY_W - DATE_LEFT_X - DATE_RIGHT_MARGIN;
    if (format_is_split(settings_get_date_format()))
    {
        x += DATE_SPLIT_INSET;
        width -= 2 * DATE_SPLIT_INSET;
    }
    return GRect(x, DATE_Y, width, DATE_H);
}

static void date_layer_update_proc(Layer *layer, GContext *ctx)
{
    const GRect bounds = layer_get_bounds(layer);

    graphics_context_set_text_color(ctx, colors_get(COLOR_DATE));
    if (s_center_buffer[0] != '\0')
    {
        draw_date_text(ctx, s_center_buffer, bounds, GTextAlignmentCenter);
        return;
    }

    draw_date_text(ctx, s_left_buffer, bounds, GTextAlignmentLeft);
    draw_date_text(ctx, s_right_buffer, bounds, GTextAlignmentRight);
}

static void refresh_date(const DateState *state)
{
    fill_buffers(state);
    if (s_date_layer)
    {
        layer_mark_dirty(s_date_layer);
    }
}

static void fill_buffers(const DateState *state)
{
    const int lang = settings_language();
    const DateFormat format = settings_get_date_format();
    const char sep = date_numeric_separator();
    const char *day_name = date_weekday_abbr(lang, state->weekday);

    s_left_buffer[0] = '\0';
    s_right_buffer[0] = '\0';
    s_center_buffer[0] = '\0';

    if (format_is_split(format))
    {
        const char *left = format == DATE_FORMAT_DAY_DAY_NAME ? state->day : day_name;
        const char *right = format == DATE_FORMAT_DAY_DAY_NAME ? day_name : state->day;
        snprintf(s_left_buffer, sizeof(s_left_buffer), "%s", left);
        snprintf(s_right_buffer, sizeof(s_right_buffer), "%s", right);
        return;
    }

    if (format == DATE_FORMAT_MONTH_DAY)
    {
        snprintf(s_center_buffer, sizeof(s_center_buffer), "%s%c%s", state->month, sep, state->day);
        return;
    }

    snprintf(s_center_buffer, sizeof(s_center_buffer), "%s%c%s", state->day, sep, state->month);
}

static bool format_is_split(DateFormat format)
{
    return format == DATE_FORMAT_DAY_NAME_DAY || format == DATE_FORMAT_DAY_DAY_NAME;
}

static void draw_date_text(GContext *ctx, const char *text, GRect bounds, GTextAlignment alignment)
{
    graphics_draw_text(
        ctx,
        text,
        fonts_get_date_font(),
        bounds,
        GTextOverflowModeWordWrap,
        alignment,
        NULL);
}
