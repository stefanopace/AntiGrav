#pragma once
#include <pebble.h>

void steps_bar_init(char slots_mask, GRect frame);
void steps_bar_deinit();
void steps_bar_mask_include(char bits);
void steps_bar_mask_exclude(char bits);