// TODO - Copyright

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


// Feedback

layer_state_t layer_state_set_user(layer_state_t state) {
  planck_ez_left_led_off();
  planck_ez_right_led_off();
  uint8_t layer = biton32(state);
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
    return state;
}
