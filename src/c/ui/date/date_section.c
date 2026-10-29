#include <pebble.h>
#include "date_section.h"

#if !(PBL_DISPLAY_WIDTH == 260 && PBL_DISPLAY_HEIGHT == 260)

#include "../../models/date.h"
#include "../../models/unobstructed.h"
#include "date_text.h"
#include "../../settings.h"
#include "../fonts.h"
#include "../colors.h"
#include "../layout.h"

#define DATE_BUFFER_SIZE 16

static Layer *s_date_layer;

static char s_left_buffer[DATE_BUFFER_SIZE];
static char s_center_buffer[DATE_BUFFER_SIZE];
static char s_right_buffer[DATE_BUFFER_SIZE];
static int s_date_subscription = -1;

static void clear_buffers(void);
static GRect date_section_frame(void);
static void date_layer_update_proc(Layer *layer, GContext *ctx);
static void refresh_date(const DateState *state);
static void fill_buffers(const DateState *state);
static void format_separated(char *buffer, size_t size, const char *a, const char *b, const char *c, char sep);

void date_section_init()
{
    clear_buffers();

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

static void clear_buffers(void)
{
    s_left_buffer[0] = '\0';
    s_center_buffer[0] = '\0';
    s_right_buffer[0] = '\0';
}

static GRect date_section_frame(void)
{
    return GRect(
        DATE_LEFT_X,
        DATE_Y,
        DATE_DISPLAY_W - DATE_LEFT_X - DATE_RIGHT_MARGIN,
        DATE_H);
}

static void date_layer_update_proc(Layer *layer, GContext *ctx)
{
    const GRect bounds = layer_get_bounds(layer);
    const GFont font = fonts_get_small_font();
    const char *buffers[] = {s_left_buffer, s_center_buffer, s_right_buffer};
    const GTextAlignment alignments[] = {
        GTextAlignmentLeft,
        GTextAlignmentCenter,
        GTextAlignmentRight,
    };

    graphics_context_set_text_color(ctx, colors_get(COLOR_DATE));
    for (int i = 0; i < 3; i++)
    {
        graphics_draw_text(
            ctx,
            buffers[i],
            font,
            bounds,
            GTextOverflowModeWordWrap,
            alignments[i],
            NULL);
    }
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
    const char *month_name = date_month_abbr(lang, state->month_index);

    clear_buffers();

    switch (format)
    {
    case DATE_FORMAT_DAY_MONTH_NAME:
        snprintf(s_left_buffer, sizeof(s_left_buffer), "%s", day_name);
        snprintf(s_center_buffer, sizeof(s_center_buffer), "%s", state->day);
        snprintf(s_right_buffer, sizeof(s_right_buffer), "%s", month_name);
        break;
    case DATE_FORMAT_DAY_MONTH_NUM:
        snprintf(s_left_buffer, sizeof(s_left_buffer), "%s", day_name);
        format_separated(s_right_buffer, sizeof(s_right_buffer), state->day, state->month, NULL, sep);
        break;
    case DATE_FORMAT_MONTH_DAY_NUM:
        snprintf(s_left_buffer, sizeof(s_left_buffer), "%s", day_name);
        format_separated(s_right_buffer, sizeof(s_right_buffer), state->month, state->day, NULL, sep);
        break;
    case DATE_FORMAT_DAY_NAME_MONTH_DAY:
        snprintf(s_center_buffer, sizeof(s_center_buffer), "%s,_%s_%s", day_name, month_name, state->day);
        break;
    case DATE_FORMAT_MONTH_DAY_YEAR:
        format_separated(s_center_buffer, sizeof(s_center_buffer), state->month, state->day, state->year, sep);
        break;
    case DATE_FORMAT_YEAR_MONTH_DAY:
        format_separated(s_center_buffer, sizeof(s_center_buffer), state->year, state->month, state->day, sep);
        break;
    case DATE_FORMAT_YEAR_DAY_MONTH:
        format_separated(s_center_buffer, sizeof(s_center_buffer), state->year, state->day, state->month, sep);
        break;
    case DATE_FORMAT_DAY_MONTH_YEAR:
    default:
        format_separated(s_center_buffer, sizeof(s_center_buffer), state->day, state->month, state->year, sep);
        break;
    }
}

static void format_separated(char *buffer, size_t size, const char *a, const char *b, const char *c, char sep)
{
    if (c != NULL)
    {
        snprintf(buffer, size, "%s%c%s%c%s", a, sep, b, sep, c);
        return;
    }

    snprintf(buffer, size, "%s%c%s", a, sep, b);
}

#endif
