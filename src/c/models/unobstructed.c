#include <pebble.h>
#include "unobstructed.h"
#include "../subscription.h"

#define UNOBSTRUCTED_MAX_SUBSCRIPTION 1

static SubSlot s_slots[UNOBSTRUCTED_MAX_SUBSCRIPTION];
static UnobstructedState s_state;
static SubList s_list = {s_slots, UNOBSTRUCTED_MAX_SUBSCRIPTION, 0, &s_state};

static Window *s_window;
static Layer *s_shifting_layer;
static Layer *s_fixed_layer;

static void apply_area(GRect area);
static void apply_live_area(AnimationProgress progress, void *context);
static void apply_settled_area(void *context);

void ag_unobstructed_init(Window *window)
{
    s_window = window;

    Layer *root = window_get_root_layer(window);
    GRect screen = layer_get_bounds(root);
    s_shifting_layer = layer_create(screen);
    layer_add_child(root, s_shifting_layer);
    s_fixed_layer = layer_create(screen);
    layer_add_child(root, s_fixed_layer);

    apply_area(layer_get_unobstructed_bounds(root));

    unobstructed_area_service_subscribe((UnobstructedAreaHandlers){
                                            .will_change = NULL,
                                            .change = apply_live_area,
                                            .did_change = apply_settled_area,
                                        },
                                        NULL);
}

void ag_unobstructed_deinit()
{
    unobstructed_area_service_unsubscribe();

    if (s_fixed_layer)
    {
        layer_destroy(s_fixed_layer);
        s_fixed_layer = NULL;
    }

    if (s_shifting_layer)
    {
        layer_destroy(s_shifting_layer);
        s_shifting_layer = NULL;
    }

    s_window = NULL;
    s_state = (UnobstructedState){0};
}

Layer *ag_unobstructed_shifting_layer()
{
    return s_shifting_layer;
}

Layer *ag_unobstructed_fixed_layer()
{
    return s_fixed_layer;
}

int ag_unobstructed_subscribe(void (*on_change)(const UnobstructedState *state))
{
    return ag_subscription_subscribe(&s_list, SUB_FN(on_change), 0, NULL, NULL);
}

int ag_unobstructed_unsubscribe(int subscription_handle)
{
    return ag_subscription_unsubscribe(&s_list, subscription_handle, NULL, NULL);
}

static void apply_area(GRect area)
{
    Layer *root = window_get_root_layer(s_window);
    GRect screen = layer_get_bounds(root);
    int16_t shift_y = (int16_t)((area.origin.y + area.size.h) - (screen.origin.y + screen.size.h));

    if (s_state.shift_y == shift_y && grect_equal(&s_state.area, &area))
    {
        return;
    }

    s_state.area = area;
    s_state.shift_y = shift_y;

    GRect frame = screen;
    frame.origin.y = (int16_t)(screen.origin.y + shift_y);
    layer_set_frame(s_shifting_layer, frame);

    ag_subscription_notify_all(&s_list);
}

static void apply_live_area(AnimationProgress progress, void *context)
{
    (void)progress;
    (void)context;
    apply_area(layer_get_unobstructed_bounds(window_get_root_layer(s_window)));
}

static void apply_settled_area(void *context)
{
    (void)context;
    apply_area(layer_get_unobstructed_bounds(window_get_root_layer(s_window)));
}
