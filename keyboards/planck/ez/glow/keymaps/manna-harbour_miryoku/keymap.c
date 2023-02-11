// TODO - Copyright

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


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
