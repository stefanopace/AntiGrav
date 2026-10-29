#include <pebble.h>
#include "battery.h"
#include "../settings.h"
#include "../subscription.h"

#define BATTERY_MAX_SUBSCRIPTION 4

static SubSlot s_slots[BATTERY_MAX_SUBSCRIPTION];
static BatteryState s_state;
static SubList s_list = {s_slots, BATTERY_MAX_SUBSCRIPTION, 0, &s_state};
static int s_low_threshold;

static void battery_init(int tag);
static void battery_deinit();
static void store_charge_state(BatteryChargeState s);
static void publish_battery(BatteryChargeState s);

int ag_battery_subscribe(void (*on_change)(const BatteryState *state))
{
    return ag_subscription_subscribe(&s_list, SUB_FN(on_change), 0, battery_init, NULL);
}

int ag_battery_unsubscribe(int subscription_handle)
{
    return ag_subscription_unsubscribe(&s_list, subscription_handle, battery_deinit, NULL);
}

static void battery_init(int tag)
{
    (void)tag;
    s_low_threshold = ag_settings_low_battery_threshold();
    store_charge_state(battery_state_service_peek());

    battery_state_service_subscribe(publish_battery);
}

static void battery_deinit()
{
    battery_state_service_unsubscribe();
}

static void store_charge_state(BatteryChargeState s)
{
    s_state.percent = s.charge_percent;
    s_state.is_charging = s.is_charging;
    s_state.is_plugged = s.is_plugged;
    s_state.is_low = s_low_threshold > 0 && (s_state.percent <= s_low_threshold);
}

static void publish_battery(BatteryChargeState s)
{
    store_charge_state(s);
    ag_subscription_notify_all(&s_list);
}
