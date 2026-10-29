#include <pebble.h>
#include "bluetooth_vibrator.h"
#include "../models/quiet_time.h"
#include "../settings.h"
#include "../models/bluetooth.h"

static bool s_quiet_time_active;
static bool s_first_check;
static int s_bluetooth_subscription = -1;
static int s_quiet_time_subscription = -1;

static void store_quiet_time(const QuietTimeState *state);
static void vibrate(const BluetoothState *state);

void bluetooth_vibrator_init()
{
    s_first_check = true;
    s_quiet_time_active = false;
    s_bluetooth_subscription = -1;
    s_quiet_time_subscription = -1;
    if (settings_vibration_pattern_on_bt_disconnect() == VIBRATION_OFF)
    {
        return;
    }

    if (settings_vibration_on_bt_disconnect_respect_quiet_time())
    {
        s_quiet_time_subscription = ag_quiet_time_subscribe(store_quiet_time);
    }
    s_bluetooth_subscription = ag_bluetooth_subscribe(vibrate);
}

void bluetooth_vibrator_deinit()
{
    s_bluetooth_subscription = ag_bluetooth_unsubscribe(s_bluetooth_subscription);
    s_quiet_time_subscription = ag_quiet_time_unsubscribe(s_quiet_time_subscription);
}

static void store_quiet_time(const QuietTimeState *state)
{
    s_quiet_time_active = state->active;
}

static void vibrate(const BluetoothState *state)
{
    if (s_first_check == true)
    {
        s_first_check = false;
        return;
    }

    if (!state->connected)
    {
        if (settings_vibration_on_bt_disconnect_respect_quiet_time() && s_quiet_time_active)
        {
        }
        else if (settings_vibration_pattern_on_bt_disconnect() == VIBRATION_SHORT)
        {
            vibes_short_pulse();
        }
        else if (settings_vibration_pattern_on_bt_disconnect() == VIBRATION_LONG)
        {
            vibes_long_pulse();
        }
        else if (settings_vibration_pattern_on_bt_disconnect() == VIBRATION_DOUBLE)
        {
            vibes_double_pulse();
        }
    }
}
