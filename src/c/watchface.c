#include <pebble.h>
#include "models/message.h"
#include "models/unobstructed.h"
#include "ui/bluetooth_vibrator.h"
#include "ui/bar/slots.h"
#include "ui/bar/status_bar.h"
#include "ui/fonts.h"
#include "settings.h"
#include "ui/clock/clock_section.h"
#include "ui/date/date_section.h"
#include "models/phone_battery.h"
#include "ui/colors.h"

static Window *s_main_window;

static void init();
static void deinit();
static void main_window_load(Window *window);
static void main_window_unload(Window *window);
static void reset();

int main(void)
{
  init();
  app_event_loop();
  deinit();
}

static void init()
{
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers){
                                                .load = main_window_load,
                                                .unload = main_window_unload});

  window_stack_push(s_main_window, true);
}

static void deinit()
{
  window_destroy(s_main_window);
}

static void main_window_load(Window *window)
{
  ag_settings_init(reset);
  ag_phone_battery_init();
  ag_unobstructed_init(window);

  colors_init(window);
  fonts_init();
  bluetooth_vibrator_init();
  status_bar_init();
  clock_section_init();
  date_section_init();
  slots_init();

  ag_phone_battery_request();
}

static void main_window_unload(Window *window)
{
  (void)window;

  slots_deinit();
  bluetooth_vibrator_deinit();
  date_section_deinit();
  clock_section_deinit();
  status_bar_deinit();
  fonts_deinit();

  ag_unobstructed_deinit();
  ag_phone_battery_deinit();
  ag_settings_deinit();
}

static void reset()
{
  // keeps app message open by keeping a fake subscriber
  int message_hold = ag_message_subscribe(NULL);
  window_stack_pop(false);
  deinit();
  init();
  ag_message_unsubscribe(message_hold);
}
