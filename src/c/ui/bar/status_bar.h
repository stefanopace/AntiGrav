#pragma once
#include <pebble.h>

typedef struct StatusBarState
{
    bool has_icons;
} StatusBarState;

void status_bar_init(void);
void status_bar_deinit();
void status_bar_set_icons_visible(bool visible);
int status_bar_subscribe(void (*on_change)(const StatusBarState *state));
int status_bar_unsubscribe(int subscription_handle);
