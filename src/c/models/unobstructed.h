#pragma once
#include <pebble.h>

typedef struct UnobstructedState
{
    GRect area;
    int16_t shift_y;
} UnobstructedState;

void ag_unobstructed_init(Window *window);
void ag_unobstructed_deinit();
Layer *ag_unobstructed_shifting_layer();
Layer *ag_unobstructed_fixed_layer();
int ag_unobstructed_subscribe(void (*on_change)(const UnobstructedState *state));
int ag_unobstructed_unsubscribe(int subscription_handle);
