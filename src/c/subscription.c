#include "subscription.h"

typedef void (*SubVisit)(SubSlot *slot, void *ctx);

typedef void (*SubStateFn)(const void *state);

typedef struct
{
    SubFilter filter;
    int value;
    const void *state;
} SubNotifyFilter;

static int sub_list_add(SubList *list, SubFn callback, int tag, SubFirst on_first);
static void sub_list_notify(SubList *list, int handle);
static void sub_list_clear_slot(SubSlot *slot);
static void sub_list_call_removed(const SubList *list, int tag, SubRemoved on_removed);
static void sub_list_each(SubList *list, SubVisit visit, void *ctx);
static void sub_list_notify_visit(SubSlot *slot, void *ctx);
static void sub_list_notify_filter_visit(SubSlot *slot, void *ctx);
static SubSlot *sub_list_get(SubList *list, int handle);
static void sub_list_call(SubSlot *slot, const void *state);

int ag_subscription_subscribe(SubList *list, SubFn callback, int tag, SubFirst on_first, SubAdded on_added)
{
    int handle = sub_list_add(list, callback, tag, on_first);
    if (handle < 0)
    {
        return handle;
    }

    if (on_added != NULL)
    {
        on_added(tag);
    }
    if (list->state != NULL)
    {
        sub_list_notify(list, handle);
    }
    return handle;
}

int ag_subscription_unsubscribe(SubList *list, int handle, SubHook on_last, SubRemoved on_removed)
{
    if (handle < 0 || handle >= list->capacity)
    {
        return -1;
    }

    if (!list->slots[handle].active)
    {
        return -1;
    }

    int tag = list->slots[handle].tag;
    sub_list_clear_slot(&list->slots[handle]);
    if (list->count > 0)
    {
        list->count--;
    }

    if (on_removed != NULL)
    {
        sub_list_call_removed(list, tag, on_removed);
    }

    if (list->count == 0 && on_last != NULL)
    {
        on_last();
    }
    return -1;
}

int ag_subscription_count(const SubList *list)
{
    return list->count;
}

void ag_subscription_notify_all(SubList *list)
{
    sub_list_each(list, sub_list_notify_visit, (void *)list->state);
}

void ag_subscription_notify_filter(SubList *list, SubFilter filter, int value)
{
    SubNotifyFilter notify = {filter, value, list->state};
    sub_list_each(list, sub_list_notify_filter_visit, &notify);
}

static int sub_list_add(SubList *list, SubFn callback, int tag, SubFirst on_first)
{
    int free_slot = -1;
    for (int i = 0; i < list->capacity; i++)
    {
        if (!list->slots[i].active)
        {
            free_slot = i;
            break;
        }
    }

    if (free_slot < 0)
    {
        return -1;
    }

    if (list->count == 0 && on_first != NULL)
    {
        on_first(tag);
    }

    list->slots[free_slot].callback = callback;
    list->slots[free_slot].tag = tag;
    list->slots[free_slot].active = true;
    list->count++;
    return free_slot;
}

static void sub_list_notify(SubList *list, int handle)
{
    SubSlot *slot = sub_list_get(list, handle);
    if (slot == NULL)
    {
        return;
    }

    sub_list_call(slot, list->state);
}

static void sub_list_clear_slot(SubSlot *slot)
{
    slot->callback = NULL;
    slot->tag = 0;
    slot->active = false;
}

static void sub_list_call_removed(const SubList *list, int tag, SubRemoved on_removed)
{
    int remaining[list->capacity];
    int remaining_count = 0;
    for (int i = 0; i < list->capacity; i++)
    {
        if (!list->slots[i].active)
        {
            continue;
        }
        remaining[remaining_count++] = list->slots[i].tag;
    }
    on_removed(tag, remaining, remaining_count);
}

static void sub_list_each(SubList *list, SubVisit visit, void *ctx)
{
    for (int i = 0; i < list->capacity; i++)
    {
        if (!list->slots[i].active)
        {
            continue;
        }

        visit(&list->slots[i], ctx);
    }
}

static void sub_list_notify_visit(SubSlot *slot, void *ctx)
{
    sub_list_call(slot, ctx);
}

static void sub_list_notify_filter_visit(SubSlot *slot, void *ctx)
{
    SubNotifyFilter *notify = ctx;
    if (!notify->filter(slot->tag, notify->value))
    {
        return;
    }

    sub_list_call(slot, notify->state);
}

static SubSlot *sub_list_get(SubList *list, int handle)
{
    if (handle < 0 || handle >= list->capacity)
    {
        return NULL;
    }

    if (!list->slots[handle].active)
    {
        return NULL;
    }

    return &list->slots[handle];
}

static void sub_list_call(SubSlot *slot, const void *state)
{
    if (slot->callback == NULL)
    {
        return;
    }

    ((SubStateFn)slot->callback)(state);
}
