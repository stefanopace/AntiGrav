#include <pebble.h>

#include "colors.h"
#include "../settings.h"

static GColor s_slots[COLOR_USER_SLOT_COUNT];
static GColor s_bar_source[SLOT_CONTENT_PHONE_BATTERY + 1];
static GColor s_placeholder[PLACEHOLDER_COLOR_COUNT];
static GColor s_bar;
static GColor s_icon[ICON_COLOR_COUNT];

static const int s_simple_slots[COLOR_COUNT] = {
    // COLOR_CLOCK_BACKGROUND
    COLOR_USER_BACKGROUND,
    // COLOR_CLOCK_HOURS
    COLOR_USER_TEXT,
    // COLOR_CLOCK_MINUTES
    COLOR_USER_TEXT,
    // COLOR_CLOCK_AM_PM
    COLOR_USER_TEXT,
    // COLOR_DATE
    COLOR_USER_TEXT,
};

static const int s_advanced_slots[COLOR_COUNT] = {
    // COLOR_CLOCK_BACKGROUND */
    COLOR_USER_ADVANCED_BACKGROUND,
    // COLOR_CLOCK_HOURS */
    COLOR_USER_HOURS,
    // COLOR_CLOCK_MINUTES */
    COLOR_USER_MINUTES,
    // COLOR_CLOCK_AM_PM */
    COLOR_USER_HOURS,
    // COLOR_DATE */
    COLOR_USER_DATE,
};

#if defined(PBL_BW)
static GColor bw_color(bool background);
#endif
static GColor colors_get_shared_bar(void);
static GColor icons_color_base(void);

void colors_init(Window *window)
{
    for (int i = 0; i < COLOR_USER_SLOT_COUNT; i++)
    {
        s_slots[i] = GColorFromHEX(settings_get_user_color_hex(i));
    }
    s_bar_source[SLOT_CONTENT_BATTERY] = GColorFromHEX(settings_get_bar_color_hex(SLOT_CONTENT_BATTERY));
    s_bar_source[SLOT_CONTENT_PHONE_BATTERY] = GColorFromHEX(settings_get_bar_color_hex(SLOT_CONTENT_PHONE_BATTERY));
    s_bar_source[SLOT_CONTENT_STEPS] = GColorFromHEX(settings_get_bar_color_hex(SLOT_CONTENT_STEPS));
    s_bar_source[SLOT_CONTENT_SECONDS] = GColorFromHEX(settings_get_bar_color_hex(SLOT_CONTENT_SECONDS));
    for (int placeholder = 0; placeholder < PLACEHOLDER_COLOR_COUNT; placeholder++)
    {
        s_placeholder[placeholder] = GColorFromHEX(settings_get_placeholder_color_hex(placeholder));
    }
    s_bar = GColorFromHEX(settings_get_shared_bar_color_hex());
    for (int icon = 0; icon < ICON_COLOR_COUNT; icon++)
    {
        s_icon[icon] = GColorFromHEX(settings_get_icon_color_hex(icon));
    }

    window_set_background_color(window, colors_get(COLOR_CLOCK_BACKGROUND));
}

GColor seconds_color_get(void)
{
#if defined(PBL_BW)
    return bw_color(false);
#endif
    if (settings_bar_colors_enabled())
    {
        return s_slots[COLOR_USER_SECONDS];
    }
    if (settings_advanced_colors_enabled())
    {
        return s_slots[COLOR_USER_HOURS];
    }
    return s_slots[COLOR_USER_TEXT];
}

GColor colors_get(ColorId id)
{
#if defined(PBL_BW)
    return bw_color(id == COLOR_CLOCK_BACKGROUND);
#endif
    if (id >= COLOR_COUNT)
    {
        return s_slots[COLOR_USER_TEXT];
    }

    const int *table = settings_advanced_colors_enabled() ? s_advanced_slots : s_simple_slots;
    const int slot = table[id];
    if (slot < 0 || slot >= COLOR_USER_SLOT_COUNT)
    {
        return s_slots[COLOR_USER_TEXT];
    }

    return s_slots[slot];
}

GColor colors_get_bar(SlotContent content)
{
#if defined(PBL_BW)
    return bw_color(false);
#endif
    if (!settings_bar_colors_enabled())
    {
        return colors_get_shared_bar();
    }
    switch (content)
    {
    case SLOT_CONTENT_PHONE_BATTERY:
    case SLOT_CONTENT_STEPS:
    case SLOT_CONTENT_SECONDS:
    case SLOT_CONTENT_BATTERY:
        return s_bar_source[content];
    default:
        return s_bar_source[SLOT_CONTENT_BATTERY];
    }
}

GColor colors_get_placeholder(int placeholder)
{
#if defined(PBL_BW)
    return bw_color(false);
#endif
    if (!settings_bar_colors_enabled())
    {
        return colors_get_shared_bar();
    }
    if (placeholder < 0 || placeholder >= PLACEHOLDER_COLOR_COUNT)
    {
        return s_placeholder[PLACEHOLDER_EMPTY];
    }
    return s_placeholder[placeholder];
}

GColor icons_color_get(IconColorId icon)
{
#if defined(PBL_BW)
    return bw_color(false);
#endif
    if (icon < ICON_COLOR_COUNT && settings_icon_colors_enabled())
    {
        return s_icon[icon];
    }
    return icons_color_base();
}

#if defined(PBL_BW)
static GColor bw_color(bool background)
{
    const bool inverted = settings_invert_colors_enabled();
    if (background)
    {
        return inverted ? GColorWhite : GColorBlack;
    }
    return inverted ? GColorBlack : GColorWhite;
}
#endif

static GColor colors_get_shared_bar(void)
{
    if (settings_advanced_colors_enabled())
    {
        return s_bar;
    }
    return s_slots[COLOR_USER_ACCENT];
}

static GColor icons_color_base(void)
{
    if (settings_bar_colors_enabled())
    {
        return s_slots[COLOR_USER_ICONS];
    }
    if (settings_advanced_colors_enabled())
    {
        return s_slots[COLOR_USER_HOURS];
    }
    return s_slots[COLOR_USER_TEXT];
}
