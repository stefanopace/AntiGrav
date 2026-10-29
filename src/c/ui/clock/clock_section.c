#include <pebble.h>

#include "../../models/unobstructed.h"
#include "../../settings.h"
#include "../bar/status_bar.h"
#include "../layout.h"
#include "big_clock.h"
#include "clock_section.h"
#include "compact_clock.h"
#include "pm_marker.h"

static int s_unobstructed_subscription = -1;
static int s_status_subscription = -1;
static int16_t s_offset;
static int16_t s_shift_from;
static int16_t s_shift_to;
static bool s_placed;
static Animation *s_shift;

static void rearrange_layers(const UnobstructedState *state);
static void make_room(const StatusBarState *state);
static void set_offset(int16_t offset);
static void cancel_shift(void);
static void move_clock(int16_t target);
static void clock_shift_update(Animation *animation, AnimationProgress progress);
static void clock_shift_stopped(Animation *animation, bool finished, void *context);

static const AnimationImplementation s_shift_implementation = {
    .update = clock_shift_update,
};

void clock_section_init()
{
    s_offset = 0;
    s_shift_from = 0;
    s_shift_to = 0;
    s_placed = false;
    s_shift = NULL;

    Layer *parent = layout_parent_layer();
    Layer *fixed = ag_unobstructed_fixed_layer();
    big_clock_init(fixed, parent);
    compact_clock_init(parent, parent);
    pm_marker_init(fixed);
    s_unobstructed_subscription = ag_unobstructed_subscribe(rearrange_layers);
    s_status_subscription = status_bar_subscribe(make_room);
}

void clock_section_deinit()
{
    cancel_shift();
    s_status_subscription = status_bar_unsubscribe(s_status_subscription);
    s_unobstructed_subscription = ag_unobstructed_unsubscribe(s_unobstructed_subscription);
    pm_marker_deinit();
    compact_clock_deinit();
    big_clock_deinit();
}

static void rearrange_layers(const UnobstructedState *state)
{
    const bool popup = state->shift_y != 0;
    const bool hours_large = !popup || settings_get_pin_popup_large() == PIN_POPUP_LARGE_HOURS;
    const bool minutes_large = !popup || settings_get_pin_popup_large() == PIN_POPUP_LARGE_MINUTES;
    big_clock_set_visible(hours_large, minutes_large);
    compact_clock_set_visible(!hours_large, !minutes_large);
}

static void make_room(const StatusBarState *state)
{
    int16_t target = state->has_icons ? CLOCK_STATUS_SHIFT : 0;
    if (!s_placed)
    {
        set_offset(target);
        s_placed = true;
        status_bar_set_icons_visible(target != 0);
        return;
    }
    move_clock(target);
}

static void set_offset(int16_t offset)
{
    s_offset = offset;
    big_clock_set_offset_x(offset);
    compact_clock_set_offset_x(offset);
    pm_marker_set_offset_x(offset);
}

static void cancel_shift(void)
{
    if (!s_shift)
    {
        return;
    }
    Animation *shift = s_shift;
    s_shift = NULL;
    animation_unschedule(shift);
}

static void move_clock(int16_t target)
{
    if (target == s_offset && !s_shift)
    {
        return;
    }
    status_bar_set_icons_visible(false);
    cancel_shift();
    s_shift_from = s_offset;
    s_shift_to = target;
    s_shift = animation_create();
    animation_set_implementation(s_shift, &s_shift_implementation);
    animation_set_handlers(s_shift, (AnimationHandlers){.stopped = clock_shift_stopped}, NULL);
    animation_schedule(s_shift);
}

static void clock_shift_update(Animation *animation, AnimationProgress progress)
{
    (void)animation;
    int16_t offset = (int16_t)(s_shift_from + ((int32_t)(s_shift_to - s_shift_from) * progress) / ANIMATION_NORMALIZED_MAX);
    set_offset(offset);
}

static void clock_shift_stopped(Animation *animation, bool finished, void *context)
{
    (void)context;
    if (finished)
    {
        set_offset(s_shift_to);
        status_bar_set_icons_visible(s_shift_to != 0);
    }
    animation_destroy(animation);
    if (s_shift == animation)
    {
        s_shift = NULL;
    }
}
