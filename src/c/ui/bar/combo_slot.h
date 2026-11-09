#pragma once
#include <pebble.h>
#include "../../settings.h"

void combo_slot_init(char mask_a, char mask_b, char mask_c, GRect frame);
void combo_slot_deinit();
void combo_slot_mask_include(ComboBarId combo, char bits);
void combo_slot_mask_exclude(ComboBarId combo, char bits);
