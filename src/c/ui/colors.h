#pragma once

#include <pebble.h>
#include "../settings.h"

typedef enum
{
    COLOR_CLOCK_BACKGROUND = 0,
    COLOR_CLOCK_HOURS = 1,
    COLOR_CLOCK_MINUTES = 2,
    COLOR_CLOCK_AM_PM = 3,
    COLOR_DATE = 4,
    COLOR_COUNT = 5,
} ColorId;

enum {
    COLOR_USER_SLOT_COUNT = 9,
    COLOR_USER_BACKGROUND = 0,
    COLOR_USER_TEXT = 1,
    COLOR_USER_ACCENT = 2,
    COLOR_USER_HOURS = 3,
    COLOR_USER_MINUTES = 4,
    COLOR_USER_SECONDS = 5,
    COLOR_USER_ICONS = 6,
    COLOR_USER_DATE = 7,
    COLOR_USER_ADVANCED_BACKGROUND = 8,
};

enum {
    PLACEHOLDER_EMPTY = 0,
    PLACEHOLDER_SECONDS = 1,
    PLACEHOLDER_ICONS = 2,
    PLACEHOLDER_COLOR_COUNT = 3,
};

typedef enum {
    ICON_COLOR_BLUETOOTH = 0,
    ICON_COLOR_QUIET_TIME = 1,
    ICON_COLOR_GOAL = 2,
    ICON_COLOR_LOW_BATTERY = 3,
    ICON_COLOR_PHONE_LOW = 4,
    ICON_COLOR_PHONE_CHARGING = 5,
    ICON_COLOR_COUNT = 6,
} IconColorId;

void colors_init(Window *window);
GColor colors_get(ColorId id);
GColor colors_get_bar(SlotContent content);
GColor colors_get_placeholder(int placeholder);
GColor icons_color_get(IconColorId icon);
GColor seconds_color_get(void);
