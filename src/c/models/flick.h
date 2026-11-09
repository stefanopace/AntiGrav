#pragma once
#include <pebble.h>

typedef struct FlickState
{
    int count;
    uint32_t chain_id;
} FlickState;

int ag_flick_subscribe(void (*on_flick)(const FlickState *state), int flick_count);
int ag_flick_unsubscribe(int subscription_handle);
