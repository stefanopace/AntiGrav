#pragma once
#include <pebble.h>

void fonts_init();
void fonts_deinit();

GFont fonts_get_date_font();
GFont fonts_get_seconds_font();
