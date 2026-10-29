#pragma once
#include <pebble.h>

typedef struct BluetoothState
{
    bool connected;
} BluetoothState;

int ag_bluetooth_subscribe(void (*on_change)(const BluetoothState *state));
int ag_bluetooth_unsubscribe(int subscription_handle);
