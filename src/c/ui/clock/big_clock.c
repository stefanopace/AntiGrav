#include <pebble.h>

#include "../../models/clock.h"
#include "../../settings.h"
#include "../colors.h"
#include "../layout.h"
#include "big_clock.h"

static const uint32_t s_digit_resources[][10] = {
    [FONT_REPLICANT] = {
        RESOURCE_ID_REPLICANT_DIGIT_0,
        RESOURCE_ID_REPLICANT_DIGIT_1,
        RESOURCE_ID_REPLICANT_DIGIT_2,
        RESOURCE_ID_REPLICANT_DIGIT_3,
        RESOURCE_ID_REPLICANT_DIGIT_4,
        RESOURCE_ID_REPLICANT_DIGIT_5,
        RESOURCE_ID_REPLICANT_DIGIT_6,
        RESOURCE_ID_REPLICANT_DIGIT_7,
        RESOURCE_ID_REPLICANT_DIGIT_8,
        RESOURCE_ID_REPLICANT_DIGIT_9,
    },
    [FONT_RACER] = {
        RESOURCE_ID_RACER_DIGIT_0,
        RESOURCE_ID_RACER_DIGIT_1,
        RESOURCE_ID_RACER_DIGIT_2,
        RESOURCE_ID_RACER_DIGIT_3,
        RESOURCE_ID_RACER_DIGIT_4,
        RESOURCE_ID_RACER_DIGIT_5,
        RESOURCE_ID_RACER_DIGIT_6,
        RESOURCE_ID_RACER_DIGIT_7,
        RESOURCE_ID_RACER_DIGIT_8,
        RESOURCE_ID_RACER_DIGIT_9,
    },
    [FONT_SAMURAI] = {
        RESOURCE_ID_SAMURAI_DIGIT_0,
        RESOURCE_ID_SAMURAI_DIGIT_1,
        RESOURCE_ID_SAMURAI_DIGIT_2,
        RESOURCE_ID_SAMURAI_DIGIT_3,
        RESOURCE_ID_SAMURAI_DIGIT_4,
        RESOURCE_ID_SAMURAI_DIGIT_5,
        RESOURCE_ID_SAMURAI_DIGIT_6,
        RESOURCE_ID_SAMURAI_DIGIT_7,
        RESOURCE_ID_SAMURAI_DIGIT_8,
        RESOURCE_ID_SAMURAI_DIGIT_9,
    },
};

typedef struct
{
    GBitmap *bitmap;
    int digit;
    bool loaded;
} DigitBitmapSlot;

static FontId s_font;
static GColor s_hours_color;
static GColor s_minutes_color;
static DigitBitmapSlot s_slots[CLOCK_DIGIT_COUNT];
static Layer *s_big_hours_layer;
static Layer *s_big_minutes_layer;
static GRect s_hours_base;
static GRect s_minutes_base;
static BitmapLayer *s_digit_layers[CLOCK_DIGIT_COUNT];
static int s_clock_subscription = -1;

static GRect row_frame(int16_t x, int16_t y, int16_t other_x);
static BitmapLayer *create_digit_layer(Layer *parent, GRect frame, GAlign alignment);
static GRect digit_frame(GRect row, int16_t x, int16_t y);
static void apply_digits(const ClockState *state);
static void apply_digit(int slot, const ClockDigit *digit);
static bool digit_bitmap_matches(int slot, int digit);
static GBitmap *digit_bitmap_load(int slot, int digit);
static bool slot_in_range(int slot);
static uint32_t digit_resource(int digit);
static void gbitmap_set_ink_color(GBitmap *bitmap, GColor ink);
static GColor slot_color(int slot);

void big_clock_init(Layer *hours_parent, Layer *minutes_parent)
{
    s_font = settings_get_font();
    s_hours_color = colors_get(COLOR_CLOCK_HOURS);
    s_minutes_color = colors_get(COLOR_CLOCK_MINUTES);

    const GRect hours = row_frame(CLOCK_HOUR_DEC_X, CLOCK_HOUR_DEC_Y, CLOCK_HOUR_UNI_X);
    const GRect minutes = row_frame(CLOCK_MIN_DEC_X, CLOCK_MIN_DEC_Y, CLOCK_MIN_UNI_X);

    s_hours_base = hours;
    s_minutes_base = minutes;
    s_big_hours_layer = layer_create(hours);
    s_big_minutes_layer = layer_create(minutes);
    layer_add_child(hours_parent, s_big_hours_layer);
    layer_add_child(minutes_parent, s_big_minutes_layer);

    s_digit_layers[CLOCK_HOUR_DEC] = create_digit_layer(s_big_hours_layer, digit_frame(hours, CLOCK_HOUR_DEC_X, CLOCK_HOUR_DEC_Y), GAlignRight);
    s_digit_layers[CLOCK_HOUR_UNI] = create_digit_layer(s_big_hours_layer, digit_frame(hours, CLOCK_HOUR_UNI_X, CLOCK_HOUR_UNI_Y), GAlignLeft);
    s_digit_layers[CLOCK_MINUTE_DEC] = create_digit_layer(s_big_minutes_layer, digit_frame(minutes, CLOCK_MIN_DEC_X, CLOCK_MIN_DEC_Y), GAlignRight);
    s_digit_layers[CLOCK_MINUTE_UNI] = create_digit_layer(s_big_minutes_layer, digit_frame(minutes, CLOCK_MIN_UNI_X, CLOCK_MIN_UNI_Y), GAlignLeft);

    s_clock_subscription = ag_clock_subscribe(apply_digits);
}

void big_clock_deinit(void)
{
    s_clock_subscription = ag_clock_unsubscribe(s_clock_subscription);

    for (int slot = 0; slot < CLOCK_DIGIT_COUNT; slot++)
    {
        bitmap_layer_destroy(s_digit_layers[slot]);
        s_digit_layers[slot] = NULL;
    }
    layer_destroy(s_big_hours_layer);
    s_big_hours_layer = NULL;
    layer_destroy(s_big_minutes_layer);
    s_big_minutes_layer = NULL;

    for (int slot = 0; slot < CLOCK_DIGIT_COUNT; slot++)
    {
        if (s_slots[slot].bitmap)
        {
            gbitmap_destroy(s_slots[slot].bitmap);
        }
        s_slots[slot] = (DigitBitmapSlot){0};
    }
}

void big_clock_set_visible(bool hours, bool minutes)
{
    layer_set_hidden(s_big_hours_layer, !hours);
    layer_set_hidden(s_big_minutes_layer, !minutes);
}

void big_clock_set_offset_x(int16_t offset)
{
    GRect hours = s_hours_base;
    GRect minutes = s_minutes_base;
    hours.origin.x = (int16_t)(s_hours_base.origin.x + offset);
    minutes.origin.x = (int16_t)(s_minutes_base.origin.x + offset);
    layer_set_frame(s_big_hours_layer, hours);
    layer_set_frame(s_big_minutes_layer, minutes);
}

static GRect row_frame(int16_t x, int16_t y, int16_t other_x)
{
    return GRect(x, y, other_x - x + DIGIT_WIDTH, DIGIT_HEIGHT);
}

static BitmapLayer *create_digit_layer(Layer *parent, GRect frame, GAlign alignment)
{
    BitmapLayer *layer = bitmap_layer_create(frame);
    bitmap_layer_set_compositing_mode(layer, GCompOpSet);
    bitmap_layer_set_background_color(layer, GColorClear);
    bitmap_layer_set_alignment(layer, alignment);
    layer_add_child(parent, bitmap_layer_get_layer(layer));
    return layer;
}

static GRect digit_frame(GRect row, int16_t x, int16_t y)
{
    return GRect((int16_t)(x - row.origin.x), (int16_t)(y - row.origin.y), DIGIT_WIDTH, DIGIT_HEIGHT);
}

static void apply_digits(const ClockState *state)
{
    for (int slot = 0; slot < CLOCK_DIGIT_COUNT; slot++)
    {
        apply_digit(slot, &state->digits[slot]);
    }
}

static void apply_digit(int slot, const ClockDigit *digit)
{
    Layer *layer = bitmap_layer_get_layer(s_digit_layers[slot]);

    layer_set_hidden(layer, !digit->visible);
    if (!digit_bitmap_matches(slot, digit->value) && digit->visible)
    {
        bitmap_layer_set_bitmap(s_digit_layers[slot], digit_bitmap_load(slot, digit->value));
    }
}

static bool digit_bitmap_matches(int slot, int digit)
{
    if (!slot_in_range(slot))
    {
        return false;
    }

    const DigitBitmapSlot *state = &s_slots[slot];
    return state->loaded && state->bitmap && state->digit == digit;
}

static GBitmap *digit_bitmap_load(int slot, int digit)
{
    if (!slot_in_range(slot))
    {
        return NULL;
    }

    DigitBitmapSlot *state = &s_slots[slot];
    if (state->loaded && state->bitmap && state->digit == digit)
    {
        return state->bitmap;
    }

    if (state->bitmap)
    {
        gbitmap_destroy(state->bitmap);
        state->bitmap = NULL;
    }

    state->bitmap = gbitmap_create_with_resource(digit_resource(digit));
    gbitmap_set_ink_color(state->bitmap, slot_color(slot));
    state->digit = digit;
    state->loaded = true;
    return state->bitmap;
}

static bool slot_in_range(int slot)
{
    return slot >= 0 && slot < CLOCK_DIGIT_COUNT;
}

static uint32_t digit_resource(int digit)
{
    if (digit < 0 || digit > 9)
    {
        return 0;
    }
    FontId font = s_font;
    if ((unsigned)font > FONT_SAMURAI)
    {
        font = FONT_REPLICANT;
    }
    return s_digit_resources[font][digit];
}

static void gbitmap_set_ink_color(GBitmap *bitmap, GColor ink)
{
    if (!bitmap)
    {
        return;
    }

    GColor *palette = gbitmap_get_palette(bitmap);
    if (!palette)
    {
        return;
    }

    size_t palette_size = 0;
    switch (gbitmap_get_format(bitmap))
    {
    case GBitmapFormat1BitPalette:
        palette_size = 2;
        break;
    case GBitmapFormat2BitPalette:
        palette_size = 4;
        break;
    case GBitmapFormat4BitPalette:
        palette_size = 16;
        break;
    default:
        return;
    }

    for (size_t i = 0; i < palette_size; i++)
    {
        if (palette[i].a != 0)
        {
            palette[i] = ink;
        }
    }
}

static GColor slot_color(int slot)
{
    return slot == CLOCK_HOUR_DEC || slot == CLOCK_HOUR_UNI ? s_hours_color : s_minutes_color;
}
