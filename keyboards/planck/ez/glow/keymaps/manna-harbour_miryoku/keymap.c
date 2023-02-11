// TODO - Copyright

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


// Feedback

void eeconfig_init_user2(void) {
  // Turn the initial led level down from 4 to 1.
  keyboard_config.led_level = 1;
  eeconfig_update_kb(keyboard_config.raw);
}

layer_state_t layer_state_set_user2(layer_state_t state) {
  planck_ez_left_led_off();
  planck_ez_right_led_off();
  uint8_t layer = get_highest_layer(state);
  switch (layer) {
    case U_BASE:
      send_string(" base ");
      break;
    case U_EXTRA:
      send_string(" extra ");
      break;
    case U_TAP:
      send_string(" tap ");
      break;
    case U_NAV:
      send_string(" nav ");
      planck_ez_left_led_on();
      break;
    case U_MOUSE:
      send_string(" mpise ");
      planck_ez_left_led_on();
      break;
    case U_MEDIA:
      send_string(" media ");
      planck_ez_left_led_on();
      break;
    case U_NUM:
      send_string(" num ");
      planck_ez_right_led_on();
      break;
    case U_SYM:
      send_string(" sym ");
      planck_ez_right_led_on();
      break;
    case U_FUN:
      send_string(" fun ");
      planck_ez_right_led_on();
      break;
    case U_BUTTON:
      send_string(" button ");
      planck_ez_left_led_on();
      planck_ez_right_led_on();
      break;
    default:
      break;
    }
    return state;
}

  // Turn the initial led level down from 4 to 1.
