#include <pebble.h>
#include "message.h"
#include "../subscription.h"

#define MESSAGE_MAX_SUBSCRIPTION 3
#define MESSAGE_INBOX_SIZE 1024
#define MESSAGE_OUTBOX_SIZE 128

static SubSlot s_slots[MESSAGE_MAX_SUBSCRIPTION];
static SubList s_list = {s_slots, MESSAGE_MAX_SUBSCRIPTION, 0, NULL};

static void message_init(int tag);
static void message_deinit();
static void inbox_received_handler(DictionaryIterator *iter, void *context);

int ag_message_subscribe(void (*on_message)(DictionaryIterator *iter))
{
    return ag_subscription_subscribe(&s_list, SUB_FN(on_message), 0, message_init, NULL);
}

int ag_message_unsubscribe(int subscription_handle)
{
    return ag_subscription_unsubscribe(&s_list, subscription_handle, message_deinit, NULL);
}

static void message_init(int tag)
{
    (void)tag;
    app_message_register_inbox_received(inbox_received_handler);
    app_message_open(MESSAGE_INBOX_SIZE, MESSAGE_OUTBOX_SIZE);
}

static void message_deinit()
{
    app_message_deregister_callbacks();
}

static void inbox_received_handler(DictionaryIterator *iter, void *context)
{
    (void)context;
    s_list.state = iter;
    ag_subscription_notify_all(&s_list);
    s_list.state = NULL;
}
