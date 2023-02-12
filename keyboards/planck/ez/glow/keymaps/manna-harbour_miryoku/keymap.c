// TODO - Copyright

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


// Flash leds

#define FLASH_LED_TICK 100

typedef struct {
  uint8_t count;
  bool running;
} flash_led_task;

uint32_t flash_led_callback(uint32_t trigger_time, void *cb_arg) {
  flash_led_task* task = (flash_led_task*)cb_arg;
  
  ++task->count;
  
  switch (task->count) {
    case 2:
    case 4:
      planck_ez_right_led_on();
      return FLASH_LED_TICK;
    case 1:
    case 3:
    case 5:
      planck_ez_right_led_off();
      return FLASH_LED_TICK;
    default:
      planck_ez_right_led_off();
      task->running = false;
      return 0;
  }
}

void flash_led(void) {
  static flash_led_task task = {0, false};
  static deferred_token token = INVALID_DEFERRED_TOKEN;

  // Halt any flash pattern in progress.
  if (task.running) {
    cancel_deferred_exec(token);
    task.running = false;
  }

  // Start new flash pattern.
  task.count = 0;
  task.running = true;
  token = defer_exec(FLASH_LED_TICK, flash_led_callback, &task);
}


// Feedback

void custom_eeconfig_init(void) {
  // Turn the initial led level down from 4 to 1.
  keyboard_config.led_level = 1;
  eeconfig_update_kb(keyboard_config.raw);
}

void custom_show_layer(uint8_t layer) {
  planck_ez_left_led_off();
  planck_ez_right_led_off();
  switch (layer) {
    case U_BASE:
    case U_EXTRA:
    case U_TAP:
      break;
    case U_NAV:
    case U_MOUSE:
    case U_MEDIA:
      planck_ez_left_led_on();
      flash_led();
      break;
    case U_NUM:
    case U_SYM:
    case U_FUN:
      planck_ez_right_led_on();
      break;
    case U_BUTTON:
      planck_ez_left_led_on();
      planck_ez_right_led_on();
      break;
    default:
      break;
    }
}
