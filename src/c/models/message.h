#pragma once
#include <pebble.h>

int ag_message_subscribe(void (*on_message)(DictionaryIterator *iter));
int ag_message_unsubscribe(int subscription_handle);
