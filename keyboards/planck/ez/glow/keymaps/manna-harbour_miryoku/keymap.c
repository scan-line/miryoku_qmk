// TODO - Copyright

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


// "Tooth" led management

#define FLASH_LED_TICK 100

bool left_led_free = true;
bool right_led_free = true;
uint8_t flash_led_tick_count = 0;
deferred_token flash_led_token = INVALID_DEFERRED_TOKEN;

void left_led_on(void) {
  left_led_free = false;
  planck_ez_left_led_on();
}

void left_led_off(void) {
  left_led_free = true;
  planck_ez_left_led_off();
}

void right_led_on(void) {
  right_led_free = false;
  planck_ez_right_led_on();
}

void right_led_off(void) {
  right_led_free = true;
  planck_ez_right_led_off();
}

uint32_t flash_led_callback(uint32_t trigger_time, void *arg) {
  ++flash_led_tick_count;
  
  switch (flash_led_tick_count) {
    case 1:
    case 3:
      if (left_led_free)
        planck_ez_left_led_on();
      if (right_led_free)
        planck_ez_right_led_on();          
      return FLASH_LED_TICK;
    case 2:
    case 4:
      if (left_led_free)
        planck_ez_left_led_off();
      if (right_led_free)
        planck_ez_right_led_off();          
      return FLASH_LED_TICK;
    default:
      flash_led_token = INVALID_DEFERRED_TOKEN;
      return 0;
  }
}

void flash_led(void) {
  // Halt any flash in progress.
  if (flash_led_token != INVALID_DEFERRED_TOKEN) {
    cancel_deferred_exec(flash_led_token);
    flash_led_token = INVALID_DEFERRED_TOKEN;
  }

  // Start new flash.
  flash_led_tick_count = 0;
  flash_led_token = defer_exec(FLASH_LED_TICK, flash_led_callback, NULL);
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

void custom_show_default_layer(uint8_t layer) {
  flash_led();
}


// Initialization

void custom_eeconfig_init(void) {
  // Turn the initial led level down from 4 to 1.
  keyboard_config.led_level = 1;
  eeconfig_update_kb(keyboard_config.raw);
}
