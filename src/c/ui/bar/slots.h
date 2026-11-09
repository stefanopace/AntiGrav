#pragma once
#include "../layout.h"
#include "../../settings.h"

void slots_init();
void slots_deinit();
void slots_mask_include(SlotPlaceholder placeholder, char bits);
void slots_mask_exclude(SlotPlaceholder placeholder, char bits);
