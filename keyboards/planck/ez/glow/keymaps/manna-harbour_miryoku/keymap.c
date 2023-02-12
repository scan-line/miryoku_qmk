// TODO - Copyright

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


// Flash leds

typedef struct {
  uint32_t tick;
  uint8_t count;
  bool running;
} flash_led_task;

uint32_t flash_led_callback(uint32_t trigger_time, void *cb_arg) {
  flash_led_task* task = (flash_led_task*)cb_arg;
  ++task->count;
  switch (task->count) {
    case 2:
    case 4:
      planck_ez_left_right_on();
      return 2*task->tick;
    case 1:
    case 3:
    case 5:
      planck_ez_left_right_off();
      return task->tick;
    default:
      planck_ez_left_right_off();
      task->running = false;
      return 0;
  }
}

void flash_led() {
  static flash_led_task task = {300, 0, false};
  static deferred_token token = INVALID_DEFERRED_TOKEN;

  // Halt any flash pattern in progress.
  if (task.running) {
    cancel_deferred_exec(token);
    task.running = false;
  }

  // Start new flash pattern.
  task = {300, 0, true};
  token = defer_exec(task.tick, flash_led_callback, (cb_arg*)&task);
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
