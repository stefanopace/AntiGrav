#include <pebble.h>
#include "bluetooth.h"
#include "../subscription.h"

#define BLUETOOTH_MAX_SUBSCRIPTION 3

static SubSlot s_slots[BLUETOOTH_MAX_SUBSCRIPTION];
static BluetoothState s_state;
static SubList s_list = {s_slots, BLUETOOTH_MAX_SUBSCRIPTION, 0, &s_state};

static void bluetooth_init(int tag);
static void bluetooth_deinit();
static void publish_connection(bool connected);

int ag_bluetooth_subscribe(void (*on_change)(const BluetoothState *state))
{
    return ag_subscription_subscribe(&s_list, SUB_FN(on_change), 0, bluetooth_init, NULL);
}

int ag_bluetooth_unsubscribe(int subscription_handle)
{
    return ag_subscription_unsubscribe(&s_list, subscription_handle, bluetooth_deinit, NULL);
}

static void bluetooth_init(int tag)
{
    (void)tag;
    s_state.connected = connection_service_peek_pebble_app_connection();

    connection_service_subscribe((ConnectionHandlers){
        .pebble_app_connection_handler = publish_connection});
}

static void bluetooth_deinit()
{
    connection_service_unsubscribe();
}

static void publish_connection(bool connected)
{
    if (s_state.connected == connected)
    {
        return;
    }
    s_state.connected = connected;

    ag_subscription_notify_all(&s_list);
}
