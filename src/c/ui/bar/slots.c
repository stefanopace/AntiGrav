#include <pebble.h>
#include "slots.h"
#include "battery_bar.h"
#include "phone_battery_bar.h"
#include "combo_slot.h"
#include "steps_bar.h"
#include "seconds_slot.h"
#include "seconds_bar.h"
#include "empty_bar.h"
#include "../../settings.h"

void slots_init()
{
    int battery_mask = 0;
    int phone_battery_mask = 0;
    int combo_a_mask = 0;
    int combo_b_mask = 0;
    int combo_c_mask = 0;
    int steps_mask = 0;
    int seconds_mask = 0;
    int seconds_bar_mask = 0;
    int none_mask = 0;
    int bit = 0;
    SlotContent content = SLOT_CONTENT_NONE;
    for (int i = 0; i < 3; i++)
    {
        content = settings_get_slot_content(i);
        switch (i)
        {
        case 0:
            bit = 4;
            break;
        case 1:
            bit = 2;
            break;
        case 2:
            bit = 1;
            break;
        default:
        }

        switch (content)
        {
        case SLOT_CONTENT_ICONS:
        case SLOT_CONTENT_NONE:
            none_mask += bit;
            break;
        case SLOT_CONTENT_BATTERY:
            battery_mask += bit;
            break;
        case SLOT_CONTENT_PHONE_BATTERY:
            phone_battery_mask += bit;
            break;
        case SLOT_CONTENT_COMBO_A:
            combo_a_mask += bit;
            break;
        case SLOT_CONTENT_COMBO_B:
            combo_b_mask += bit;
            break;
        case SLOT_CONTENT_COMBO_C:
            combo_c_mask += bit;
            break;
        case SLOT_CONTENT_STEPS:
            steps_mask += bit;
            break;
        case SLOT_CONTENT_SECONDS:
            seconds_mask += bit;
            break;
        case SLOT_CONTENT_SECONDS_BAR:
            seconds_bar_mask += bit;
            break;
        default:
        }
    }

    GRect slots_frame = GRect(SLOTS_X, SLOTS_Y, SLOTS_W, SLOTS_H);
    battery_bar_init(battery_mask, slots_frame);
    phone_battery_bar_init(phone_battery_mask, slots_frame);
    combo_slot_init(combo_a_mask, combo_b_mask, combo_c_mask, slots_frame);
    steps_bar_init(steps_mask, slots_frame);
    seconds_slot_init(seconds_mask, slots_frame);
    seconds_bar_init(seconds_bar_mask, slots_frame);
    empty_bar_init(none_mask, slots_frame);
}

void slots_deinit()
{
    empty_bar_deinit();
    seconds_bar_deinit();
    seconds_slot_deinit();
    steps_bar_deinit();
    combo_slot_deinit();
    phone_battery_bar_deinit();
    battery_bar_deinit();
}