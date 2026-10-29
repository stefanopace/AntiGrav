#include <pebble.h>
#include "date_section.h"

#if PBL_DISPLAY_WIDTH == 260 && PBL_DISPLAY_HEIGHT == 260

#include "../../models/date.h"
#include "../../models/unobstructed.h"
#include "date_text.h"
#include "../../settings.h"
#include "../fonts.h"
#include "../colors.h"
#include "../layout.h"

#define GABBRO_DATE_BUFFER_SIZE 8

static Layer *s_above_layer;
static Layer *s_below_layer;
static char s_above_buffer[GABBRO_DATE_BUFFER_SIZE];
static char s_below_buffer[GABBRO_DATE_BUFFER_SIZE];
static int s_date_subscription = -1;

static Layer *create_band(int y);
static void refresh_date(const DateState *state);
static GRect band_frame(int y);
static void band_update_proc(Layer *layer, GContext *ctx);
static void fill_band(char *buffer, size_t size, GabbroDateContent content, const DateState *state);

void date_section_init()
{
    s_above_buffer[0] = '\0';
    s_below_buffer[0] = '\0';
    s_above_layer = create_band(DATE_ABOVE_Y);
    s_below_layer = create_band(DATE_BELOW_Y);
    s_date_subscription = ag_date_subscribe(refresh_date);
}

void date_section_deinit()
{
    s_date_subscription = ag_date_unsubscribe(s_date_subscription);
    layer_destroy(s_above_layer);
    layer_destroy(s_below_layer);
    s_above_layer = NULL;
    s_below_layer = NULL;
}

static Layer *create_band(int y)
{
    Layer *layer = layer_create(band_frame(y));
    layer_set_update_proc(layer, band_update_proc);
    layer_add_child(ag_unobstructed_fixed_layer(), layer);
    return layer;
}

static void refresh_date(const DateState *state)
{
    fill_band(s_above_buffer, sizeof(s_above_buffer), settings_get_gabbro_date_above(), state);
    fill_band(s_below_buffer, sizeof(s_below_buffer), settings_get_gabbro_date_below(), state);
    if (s_above_layer)
    {
        layer_mark_dirty(s_above_layer);
    }
    if (s_below_layer)
    {
        layer_mark_dirty(s_below_layer);
    }
}

static GRect band_frame(int y)
{
    return GRect(
        DATE_LEFT_X,
        y,
        DATE_DISPLAY_W - DATE_LEFT_X - DATE_RIGHT_MARGIN,
        DATE_H);
}

static void band_update_proc(Layer *layer, GContext *ctx)
{
    const char *text = layer == s_above_layer ? s_above_buffer : s_below_buffer;

    graphics_context_set_text_color(ctx, colors_get(COLOR_DATE));
    graphics_draw_text(
        ctx,
        text,
        fonts_get_small_font(),
        layer_get_bounds(layer),
        GTextOverflowModeWordWrap,
        GTextAlignmentCenter,
        NULL);
}

static void fill_band(char *buffer, size_t size, GabbroDateContent content, const DateState *state)
{
    const int lang = settings_language();
    const char sep = date_numeric_separator();

    switch (content)
    {
    case GABBRO_DATE_DAY_NAME:
        snprintf(buffer, size, "%s", date_weekday_abbr(lang, state->weekday));
        break;
    case GABBRO_DATE_MONTH_NAME:
        snprintf(buffer, size, "%s", date_month_abbr(lang, state->month_index));
        break;
    case GABBRO_DATE_DAY_MONTH:
        snprintf(buffer, size, "%s%c%s", state->day, sep, state->month);
        break;
    case GABBRO_DATE_MONTH_DAY:
        snprintf(buffer, size, "%s%c%s", state->month, sep, state->day);
        break;
    case GABBRO_DATE_DAY:
    default:
        snprintf(buffer, size, "%s", state->day);
        break;
    }
}


#endif
