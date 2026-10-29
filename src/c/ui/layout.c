#include <pebble.h>

#include "layout.h"
#include "../models/unobstructed.h"

Layer *layout_parent_layer()
{
#if PBL_DISPLAY_WIDTH == 260 && PBL_DISPLAY_HEIGHT == 260
    return ag_unobstructed_fixed_layer();
#else
    return ag_unobstructed_shifting_layer();
#endif
}
