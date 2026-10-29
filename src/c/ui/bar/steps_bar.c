#include <pebble.h>

#include "../colors.h"
#include "../../settings.h"
#include "../../models/steps.h"
#include "../layout.h"
#include "steps_bar.h"
#include "generic/continuous_bar.h"
#include "generic/segmented_bar.h"

static int s_steps;
static int s_goal;
static char s_slot_mask;
static Layer *s_layer;
static int s_steps_subscription = -1;

static void steps_slot_update_proc(Layer *layer, GContext *ctx);
static void update_steps(const StepsState *state);

void steps_bar_init(char slot_mask, GRect frame)
{
    s_layer = NULL;
    s_steps_subscription = -1;
    if (slot_mask == 0)
    {
        return;
    }
    s_slot_mask = slot_mask;
    s_steps = 0;
    s_goal = 0;

    s_layer = layer_create(frame);
    layer_set_update_proc(s_layer, steps_slot_update_proc);
    layer_add_child(layout_parent_layer(), s_layer);

    s_steps_subscription = ag_steps_subscribe(update_steps);
}

void steps_bar_deinit(void)
{
    s_steps_subscription = ag_steps_unsubscribe(s_steps_subscription);
    if (s_layer)
    {
        layer_destroy(s_layer);
        s_layer = NULL;
    }
}

static void steps_slot_update_proc(Layer *layer, GContext *ctx)
{
    const BarStyle style = settings_get_bar_style(SLOT_CONTENT_STEPS);
    const bool mirrored = settings_get_bar_mirrored(SLOT_CONTENT_STEPS);
    const GColor color = colors_get_bar(SLOT_CONTENT_STEPS);
    int goal = s_goal;
    int steps = s_steps;
    if (goal <= 0)
    {
        steps = 0;
        goal = 1;
    }
    if (style == CONTINUOUS)
    {
        continuous_bar_render(layer, ctx, steps, goal, s_slot_mask, true, mirrored, color);
    }
    else
    {
        segmented_bar_render(layer, ctx, steps, goal, s_slot_mask, SEGMENTED_BAR_FLOOR, true, BAR_Y, BAR_H, SEGMENTED_BAR_EMPTY_H, mirrored, color);
    }
}

static void update_steps(const StepsState *state)
{
    s_steps = state->steps_amount;
    s_goal = state->steps_goal;
    layer_mark_dirty(s_layer);
}
