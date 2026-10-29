#include <pebble.h>
#include "phone_battery.h"
#include "bluetooth.h"
#include "message_keys.auto.h"
#include "message.h"
#include "../subscription.h"
#include "../settings.h"

#define PHONE_BATTERY_MAX_SUBSCRIPTION 3

enum
{
    PERSIST_KEY_PHONE_BATTERY = 3,
};

typedef struct
{
    int percent;
    bool is_charging;
    bool available;
} PhoneBatteryCache;

static SubSlot s_slots[PHONE_BATTERY_MAX_SUBSCRIPTION];
static PhoneBatteryState s_state;
static SubList s_list = {s_slots, PHONE_BATTERY_MAX_SUBSCRIPTION, 0, &s_state};
static int s_low_threshold;
static int s_message_subscription = -1;
static int s_bluetooth_subscription = -1;

static void phone_communications(DictionaryIterator *iter);
static void phone_battery_lazy_init(int tag);
static void phone_battery_release_lazy(void);
static void phone_battery_load(void);
static void set_unavailable(const BluetoothState *state);
static void phone_battery_save(void);

void ag_phone_battery_init()
{
    s_message_subscription = ag_message_subscribe(phone_communications);
}

void ag_phone_battery_deinit()
{
    s_message_subscription = ag_message_unsubscribe(s_message_subscription);
}

void ag_phone_battery_request()
{
    DictionaryIterator *iter = NULL;
    if (app_message_outbox_begin(&iter) != APP_MSG_OK)
    {
        return;
    }

    int request = ag_subscription_count(&s_list) > 0 ? 1 : 0;
    dict_write_int(iter, MESSAGE_KEY_PhoneBatteryRequest, &request, sizeof(int), true);
    dict_write_end(iter);
    app_message_outbox_send();
}

int ag_phone_battery_subscribe(void (*on_change)(const PhoneBatteryState *state))
{
    return ag_subscription_subscribe(&s_list, SUB_FN(on_change), 0, phone_battery_lazy_init, NULL);
}

int ag_phone_battery_unsubscribe(int subscription_handle)
{
    return ag_subscription_unsubscribe(&s_list, subscription_handle, phone_battery_release_lazy, NULL);
}

static void phone_communications(DictionaryIterator *iter)
{
    // The phone sends this once PKJS is listening. Reply with 1 or 0.
    if (dict_find(iter, MESSAGE_KEY_PhoneBatteryRequest))
    {
        ag_phone_battery_request();
        return;
    }

    Tuple *percent_t = dict_find(iter, MESSAGE_KEY_PhoneBattery);
    if (!percent_t)
    {
        return;
    }

    bool is_charging = false;
    Tuple *charging_t = dict_find(iter, MESSAGE_KEY_PhoneBatteryCharging);
    if (charging_t)
    {
        is_charging = charging_t->value->int32 != 0;
    }

    int percent = (int)percent_t->value->int32;
    if (percent < 0)
    {
        percent = 0;
    }
    else if (percent > 100)
    {
        percent = 100;
    }

    const bool is_low = s_low_threshold > 0 && percent <= s_low_threshold;
    if (s_state.available && s_state.percent == percent && s_state.is_charging == is_charging && s_state.is_low == is_low)
    {
        return;
    }

    s_state.percent = percent;
    s_state.is_charging = is_charging;
    s_state.is_low = is_low;
    s_state.available = true;
    ag_subscription_notify_all(&s_list);
}

static void phone_battery_lazy_init(int tag)
{
    (void)tag;
    s_low_threshold = ag_settings_phone_low_battery_threshold();
    phone_battery_load();
    s_state.is_low = s_state.available && s_low_threshold > 0 && s_state.percent <= s_low_threshold;

    s_bluetooth_subscription = ag_bluetooth_subscribe(set_unavailable);
}

static void phone_battery_release_lazy(void)
{
    phone_battery_save();
    s_state.percent = 0;
    s_state.is_charging = false;
    s_state.is_low = false;
    s_state.available = false;
    s_bluetooth_subscription = ag_bluetooth_unsubscribe(s_bluetooth_subscription);
}

static void phone_battery_load(void)
{
    PhoneBatteryCache cache;
    if (!persist_exists(PERSIST_KEY_PHONE_BATTERY) ||
        persist_get_size(PERSIST_KEY_PHONE_BATTERY) != (int)sizeof(cache))
    {
        return;
    }

    persist_read_data(PERSIST_KEY_PHONE_BATTERY, &cache, sizeof(cache));
    if (cache.percent < 0)
    {
        cache.percent = 0;
    }
    else if (cache.percent > 100)
    {
        cache.percent = 100;
    }

    s_state.percent = cache.percent;
    s_state.is_charging = cache.is_charging;
    s_state.available = cache.available;
}

static void set_unavailable(const BluetoothState *state)
{
    if (state->connected)
    {
        return;
    }

    s_state.available = false;
    ag_subscription_notify_all(&s_list);
}

static void phone_battery_save(void)
{
    PhoneBatteryCache cache;
    cache.percent = s_state.percent;
    cache.is_charging = s_state.is_charging;
    cache.available = s_state.available;
    persist_write_data(PERSIST_KEY_PHONE_BATTERY, &cache, sizeof(cache));
}
