// TODO - Copyright

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


// "Tooth" led management

#define FLASH_LED_TICK 100

bool left_led_available = true;
bool right_led_available = true;
bool flash_led_running = false;

void left_led_on(void) {
  left_led_available = false;
  planck_ez_left_led_on();
}

void left_led_off(void) {
  left_led_available = true;
  planck_ez_left_led_off();
}

void right_led_on(void) {
  right_led_available = false;
  planck_ez_right_led_on();
}

void right_led_off(void) {
  right_led_available = true;
  planck_ez_right_led_off();
}

uint32_t flash_led_callback(uint32_t trigger_time, void *arg) {
  uint8_t* count = (flash_led_task*)arg;
  
  ++*count;
  
  switch (*count) {
    case 2:
    case 4:
      if (left_led_available)
        planck_ez_left_led_on();
      if (right_led_available)
        planck_ez_right_led_on();          
      return FLASH_LED_TICK;
    case 1:
    case 3:
    case 5:
      if (left_led_available)
        planck_ez_left_led_off();
      if (right_led_available)
        planck_ez_right_led_off();          
      return FLASH_LED_TICK;
    default:
      flash_led = false;
      return 0;
  }
}

void flash_led(void) {
  static count = 0;
  static deferred_token token = INVALID_DEFERRED_TOKEN;

  // Halt any flash in progress.
  if (flash_led_running) {
    cancel_deferred_exec(token);
    flash_led_running = false;
  }

  // Start new flash.
  count = 0;
  flash_led_running = true;
  token = defer_exec(FLASH_LED_TICK, flash_led_callback, &count);
  if (token == INVALID_DEFERRED_TOKEN) {
    flash_led_running = false;
  }
}


// Feedback

void custom_show_layer(uint8_t layer) {
  switch (layer) {
    case U_BASE:
    case U_EXTRA:
    case U_TAP:
      left_led_off();
      right_led_off();
      break;
    case U_NAV:
    case U_MOUSE:
    case U_MEDIA:
      left_led_on();
      right_led_off();
      break;
    case U_NUM:
    case U_SYM:
    case U_FUN:
      left_led_off();
      right_led_on();
      break;
    case U_BUTTON:
      left_led_on();
      right_led_on();
      break;
    default:
      left_led_off();
      right_led_off();
      break;
    }
}

void custom_show_layer(uint8_t layer) {
  flash_led();
}


// Initialization

void custom_eeconfig_init(void) {
  // Turn the initial led level down from 4 to 1.
  keyboard_config.led_level = 1;
  eeconfig_update_kb(keyboard_config.raw);
}
