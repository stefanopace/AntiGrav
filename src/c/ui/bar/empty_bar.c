#include <pebble.h>

#include "../colors.h"
#include "../layout.h"
#include "empty_bar.h"
#include "generic/continuous_bar.h"

static char s_slot_mask;
static Layer *s_layer;

static void empty_slot_update_proc(Layer *layer, GContext *ctx);

void empty_bar_init(char slot_mask, GRect frame)
{
    s_layer = NULL;
    if (slot_mask == 0)
    {
        return;
    }
    s_slot_mask = slot_mask;

    s_layer = layer_create(frame);
    layer_set_update_proc(s_layer, empty_slot_update_proc);
    layer_add_child(layout_parent_layer(), s_layer);
}

void empty_bar_deinit(void)
{
    if (s_layer)
    {
        layer_destroy(s_layer);
        s_layer = NULL;
    }
}

static void empty_slot_update_proc(Layer *layer, GContext *ctx)
{
    continuous_bar_render(layer, ctx, 0, 1, s_slot_mask, true, false, colors_get_placeholder(PLACEHOLDER_EMPTY));
}