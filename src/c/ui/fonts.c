#include <pebble.h>

#include "fonts.h"
#include "../settings.h"

static GFont s_date_font;
static GFont s_seconds_font;

static uint32_t date_font_resource(FontId font);
static uint32_t seconds_font_resource(FontId font);

void fonts_init()
{
    const FontId font = settings_get_font();
    s_date_font = fonts_load_custom_font(resource_get_handle(date_font_resource(font)));
    s_seconds_font = fonts_load_custom_font(resource_get_handle(seconds_font_resource(font)));
}

void fonts_deinit()
{
    if (s_date_font)
    {
        fonts_unload_custom_font(s_date_font);
        s_date_font = NULL;
    }
    if (s_seconds_font)
    {
        fonts_unload_custom_font(s_seconds_font);
        s_seconds_font = NULL;
    }
}

GFont fonts_get_date_font() { return s_date_font; }

GFont fonts_get_seconds_font() { return s_seconds_font; }

static uint32_t date_font_resource(FontId font)
{
    switch (font)
    {
    case FONT_RACER:
        return RESOURCE_ID_FONT_RACER_32;
    case FONT_SAMURAI:
        return RESOURCE_ID_FONT_SAMURAI_32;
    case FONT_REPLICANT:
    default:
        return RESOURCE_ID_FONT_REPLICANT_32;
    }
}

static uint32_t seconds_font_resource(FontId font)
{
    switch (font)
    {
    case FONT_RACER:
        return RESOURCE_ID_FONT_RACER_SECONDS_16;
    case FONT_SAMURAI:
        return RESOURCE_ID_FONT_SAMURAI_SECONDS_16;
    case FONT_REPLICANT:
    default:
        return RESOURCE_ID_FONT_REPLICANT_SECONDS_16;
    }
}
