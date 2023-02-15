// TODO - Copyright

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


// "Tooth" led management

#define FLASH_LED_TICK 50

struct {
  bool left;
  bool right;
  bool suspended;
} led;

struct {
  deferred_token token;
  const uint8_t* pattern;
  bool on;
} led_flash = {INVALID_DEFERRED_TOKEN};

void left_led_on(void) {
  led.left = true;
  if (!led.suspended)
    planck_ez_left_led_on();
}

void left_led_off(void) {
  led.left = false;
  planck_ez_left_led_off();
}

void right_led_on(void) {
  led.right = true;
  if (!led.suspended)
    planck_ez_right_led_on();
}

void right_led_off(void) {
  led.right = false;
  planck_ez_right_led_off();
}

uint32_t flash_led_callback(uint32_t trigger_time, void *arg) {
  // Flash unused leds
  bool flash_left = !led.left;
  bool flash_right = !led.right;
  
  uint8_t duration = *led_flash.pattern * FLASH_LED_TICK;
  if (duration == 0) {
    // End of array
    // Turn off and stop
    if (flash_left || led.suspended)
      planck_ez_left_led_off();
    if (flash_right || led.suspended)
      planck_ez_right_led_off();          
    led_flash.token = INVALID_DEFERRED_TOKEN;
    return duration;
  }

  if (led_flash.on) {
    // Turn on
    if (flash_left)
      planck_ez_left_led_on();
    if (flash_right)
      planck_ez_right_led_on();          
  } else {
    // Turn off
    if (flash_left)
      planck_ez_left_led_off();
    if (flash_right)
      planck_ez_right_led_off();          
  }
  
  // Next state
  led_flash.on = !led_flash.on;
  ++led_flash.pattern;
  return duration;
}

void flash_led(const uint8_t* pattern) {
  // Halt any flash in progress
  if (led_flash.token != INVALID_DEFERRED_TOKEN) {
    cancel_deferred_exec(led_flash.token);
    led_flash.token = INVALID_DEFERRED_TOKEN;
  }

  // Start new flash
  led_flash.pattern = pattern;
  led_flash.on = true;
  led_flash.token = defer_exec(FLASH_LED_TICK, flash_led_callback, NULL);
}

void suspend_power_down_user(void) {
  // May be run multiple times on suspend
  led.suspended = true;
  planck_ez_left_led_off();
  planck_ez_right_led_off();
}

void suspend_wakeup_init_user(void) {
  led.suspended = false;
  if (led.left)
    planck_ez_left_led_on();
  if (led.right)
    planck_ez_right_led_on();
}


// Feedback

// On/off timings - must end in 0
const uint8_t mode_flash[] = {1, 1, 1, 0};
const uint8_t default_layer_flash[] = {1, 1, 1, 0};
const uint8_t toggle_on_flash[] = {1, 1, 1, 0};
const uint8_t toggle_off_flash[] = {3, 0};
const uint8_t detent_flash[] = {1, 0};

void show_mode_custom(uint16_t keycode) {
  flash_led(mode_flash);
}

void show_layer_custom(uint8_t layer) {
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

void show_default_layer_custom(uint8_t layer) {
  flash_led(default_layer_flash);
}

void show_toggle_custom(uint16_t keycode, bool value) {
  if (value)
    flash_led(toggle_on_flash);
  else
    flash_led(toggle_off_flash);
}

void show_value_custom(uint16_t keycode, uint16_t value, bool detent) {
  if (detent)
    flash_led(detent_flash);
}


// Initialization

void eeconfig_init_custom(void) {
  // Turn the initial led level down from 4 to 1
  keyboard_config.led_level = 1;
  eeconfig_update_kb(keyboard_config.raw);
}
