#include <pebble.h>

#include "fonts.h"
#include "../settings.h"

static GFont s_small_font;

static uint32_t small_font_resource(FontId font);

void fonts_init()
{
    s_small_font = fonts_load_custom_font(resource_get_handle(small_font_resource(settings_get_font())));
}

void fonts_deinit()
{
    if (s_small_font)
    {
        fonts_unload_custom_font(s_small_font);
        s_small_font = NULL;
    }
}

GFont fonts_get_small_font() { return s_small_font; }

static uint32_t small_font_resource(FontId font)
{
    switch (font)
    {
    case FONT_RACER:
#if PBL_DISPLAY_WIDTH == 144 && PBL_DISPLAY_HEIGHT == 168
        return RESOURCE_ID_FONT_RACER_13;
#else
        return RESOURCE_ID_FONT_RACER_16;
#endif
    case FONT_SAMURAI:
#if PBL_DISPLAY_WIDTH == 144 && PBL_DISPLAY_HEIGHT == 168
        return RESOURCE_ID_FONT_SAMURAI_13;
#else
        return RESOURCE_ID_FONT_SAMURAI_16;
#endif
    case FONT_REPLICANT:
    default:
#if PBL_DISPLAY_WIDTH == 144 && PBL_DISPLAY_HEIGHT == 168
        return RESOURCE_ID_FONT_REPLICANT_13;
#else
        return RESOURCE_ID_FONT_REPLICANT_16;
#endif
    }
}
