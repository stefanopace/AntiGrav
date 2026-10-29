#pragma once
#include <pebble.h>

typedef void (*SubFn)(void);
typedef void (*SubHook)(void);
typedef void (*SubFirst)(int tag);
typedef void (*SubAdded)(int tag);
typedef void (*SubRemoved)(int tag, const int *remaining, int remaining_count);

typedef struct SubSlot
{
    SubFn callback;
    int tag;
    bool active;
} SubSlot;

typedef struct SubList
{
    SubSlot *slots;
    int capacity;
    int count;
    const void *state;
} SubList;

typedef bool (*SubFilter)(int tag, int value);

#define SUB_FN(fn) ((SubFn)(fn))

int ag_subscription_subscribe(SubList *list, SubFn callback, int tag, SubFirst on_first, SubAdded on_added);
int ag_subscription_unsubscribe(SubList *list, int handle, SubHook on_last, SubRemoved on_removed);
int ag_subscription_count(const SubList *list);
void ag_subscription_notify_all(SubList *list);
void ag_subscription_notify_filter(SubList *list, SubFilter filter, int value);
