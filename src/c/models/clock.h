#pragma once

#include <pebble.h>

typedef enum
{
    CLOCK_HOUR_DEC = 0,
    CLOCK_HOUR_UNI,
    CLOCK_MINUTE_DEC,
    CLOCK_MINUTE_UNI,
    CLOCK_DIGIT_COUNT
} ClockDigitId;

typedef struct ClockDigit
{
    int value;
    bool visible;
} ClockDigit;

#define CLOCK_TEXT_SIZE 3

typedef struct ClockState
{
    ClockDigit digits[CLOCK_DIGIT_COUNT];
    bool is_pm;
    char hours[CLOCK_TEXT_SIZE];
    char minutes[CLOCK_TEXT_SIZE];
} ClockState;

int ag_clock_subscribe(void (*on_change)(const ClockState *state));
int ag_clock_unsubscribe(int subscription_handle);
