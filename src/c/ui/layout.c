#include <pebble.h>

#include "layout.h"
#include "../models/unobstructed.h"

Layer *layout_parent_layer()
{
    return ag_unobstructed_shifting_layer();
}
