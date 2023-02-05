// Copyright 2022 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


#if 1

// Additional keycodes

enum user_keycodes {
  U_RGB_OFF = SAFE_RANGE,
  U_WIN,
  U_MAC,
  U_LNX,
  U_SAFE_RANGE,
};


// OS-specific clipboard + modifier swap

typedef enum {
    OS_MODE_WIN,
    OS_MODE_MAC,
    OS_MODE_LNX,
} os_mode_t;

#define OS_DEFAULT_MODE OS_MODE_WIN

// Windows clipboard
#define OS_CLIP_CUT_WIN LCTL(KC_X)
#define OS_CLIP_CPY_WIN LCTL(KC_C)
#define OS_CLIP_PST_WIN LCTL(KC_V)
#define OS_CLIP_UND_WIN LCTL(KC_Z)
#define OS_CLIP_RDO_WIN LCTL(KC_Y)

// Mac clipboard
#define OS_CLIP_CUT_MAC LCMD(KC_X)
#define OS_CLIP_CPY_MAC LCMD(KC_C)
#define OS_CLIP_PST_MAC LCMD(KC_V)
#define OS_CLIP_UND_MAC LCMD(KC_Z)
#define OS_CLIP_RDO_MAC SCMD(KC_Z)

typedef enum {
    OS_CLIP_CUT,
    OS_CLIP_CPY,
    OS_CLIP_PST,
    OS_CLIP_UND,
    OS_CLIP_RDO,
    OS_CLIP_END,
} os_clip_t;

// Windows and Mac only
// Linux keycodes are left as-is
const uint16_t PROGMEM os_keycodes[][OS_CLIP_END] = {
  [OS_MODE_WIN] = { OS_CLIP_CUT_WIN, OS_CLIP_CPY_WIN, OS_CLIP_PST_WIN, OS_CLIP_UND_WIN, OS_CLIP_RDO_WIN, },
  [OS_MODE_MAC] = { OS_CLIP_CUT_MAC, OS_CLIP_CPY_MAC, OS_CLIP_PST_MAC, OS_CLIP_UND_MAC, OS_CLIP_RDO_MAC, },
};

// TODO - save/restore eeprom
static os_mode_t os_mode = OS_DEFAULT_MODE;

#endif


// Additional Features double tap guard

enum {
    U_TD_BOOT,
#define MIRYOKU_X(LAYER, STRING) U_TD_U_##LAYER,
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};

void u_td_fn_boot(qk_tap_dance_state_t *state, void *user_data) { \
  if (state->count == 2) {
    reset_keyboard();
  }
}

#define MIRYOKU_X(LAYER, STRING) \
void u_td_fn_U_##LAYER(qk_tap_dance_state_t *state, void *user_data) { \
  if (state->count == 2) { \
    default_layer_set((layer_state_t)1 << U_##LAYER); \
  } \
}
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X

qk_tap_dance_action_t tap_dance_actions[] = {
    [U_TD_BOOT] = ACTION_TAP_DANCE_FN(u_td_fn_boot),
#define MIRYOKU_X(LAYER, STRING) [U_TD_U_##LAYER] = ACTION_TAP_DANCE_FN(u_td_fn_U_##LAYER),
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};


// keymap

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
#define MIRYOKU_X(LAYER, STRING) [U_##LAYER] = U_MACRO_VA_ARGS(MIRYOKU_LAYERMAPPING_##LAYER, MIRYOKU_LAYER_##LAYER),
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};


#if 0

// shift functions

const key_override_t capsword_key_override = ko_make_basic(MOD_MASK_SHIFT, CW_TOGG, KC_CAPS);

const key_override_t **key_overrides = (const key_override_t *[]){
    &capsword_key_override,
    NULL
};

#else

// modifier functions

#define LAYER_MASK_NUM (1 << U_NUM)

const key_override_t capsword_key_override = ko_make_basic(MOD_MASK_SHIFT, CW_TOGG, KC_CAPS);

const key_override_t dot_key_override = ko_make_with_layers(MOD_MASK_SHIFT, KC_DOT, KC_LEFT_PAREN, LAYER_MASK_NUM);
const key_override_t nine_key_override = ko_make_with_layers(MOD_MASK_SHIFT, KC_9, KC_LEAD, LAYER_MASK_NUM);

const key_override_t **key_overrides = (const key_override_t *[]){
  &capsword_key_override,
  &dot_key_override,
  &nine_key_override,
  NULL
};


// Custom key processing

bool process_os_mode_key(os_mode_t mode, keyrecord_t *record) {
  if (record->event.pressed) {
    os_mode = mode;
    if (mode == OS_MODE_MAC) {
        process_magic(MAGIC_SWAP_CTL_GUI, record);
    } else {
        process_magic(MAGIC_UNSWAP_CTL_GUI, record);
    }
  }
  return false;
}

bool process_os_clip_key(os_clip_t clip, keyrecord_t *record) {
  // Linux keycodes are passed through as-is.
  if (os_mode == OS_MODE_LNX) {
    return true;
  }
  // Windows and Mac keycodes are translated.
  if (record->event.pressed) {
    register_code16(os_keycodes[os_mode][clip]);
  } else {
    unregister_code16(os_keycodes[os_mode][clip]);
  }
  return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case U_WIN:
      return process_os_mode_key(OS_MODE_WIN, record);
    case U_MAC:
      return process_os_mode_key(OS_MODE_MAC, record);
    case U_LNX:
      return process_os_mode_key(OS_MODE_LNX, record);
    case U_CUT:
      return process_os_clip_key(OS_CLIP_CUT, record);
    case U_CPY:
      return process_os_clip_key(OS_CLIP_CPY, record);
    case U_PST:
      return process_os_clip_key(OS_CLIP_PST, record);
    case U_UND:
      return process_os_clip_key(OS_CLIP_UND, record);
    case U_RDO:
      return process_os_clip_key(OS_CLIP_RDO, record);
    default:
      return true;
  }
}

#endif


// thumb combos

#if defined (MIRYOKU_KLUDGE_THUMBCOMBOS)
const uint16_t PROGMEM thumbcombos_base_right[] = {LT(U_SYM, KC_ENT), LT(U_NUM, KC_BSPC), COMBO_END};
const uint16_t PROGMEM thumbcombos_base_left[] = {LT(U_NAV, KC_SPC), LT(U_MOUSE, KC_TAB), COMBO_END};
const uint16_t PROGMEM thumbcombos_nav[] = {KC_ENT, KC_BSPC, COMBO_END};
const uint16_t PROGMEM thumbcombos_mouse[] = {KC_BTN2, KC_BTN1, COMBO_END};
const uint16_t PROGMEM thumbcombos_media[] = {KC_MSTP, KC_MPLY, COMBO_END};
const uint16_t PROGMEM thumbcombos_num[] = {KC_0, KC_MINS, COMBO_END};
  #if defined (MIRYOKU_LAYERS_FLIP)
const uint16_t PROGMEM thumbcombos_sym[] = {KC_UNDS, KC_LPRN, COMBO_END};
  #else
const uint16_t PROGMEM thumbcombos_sym[] = {KC_RPRN, KC_UNDS, COMBO_END};
  #endif
const uint16_t PROGMEM thumbcombos_fun[] = {KC_SPC, KC_TAB, COMBO_END};
combo_t key_combos[COMBO_COUNT] = {
  COMBO(thumbcombos_base_right, LT(U_FUN, KC_DEL)),
  COMBO(thumbcombos_base_left, LT(U_MEDIA, KC_ESC)),
  COMBO(thumbcombos_nav, KC_DEL),
  COMBO(thumbcombos_mouse, KC_BTN3),
  COMBO(thumbcombos_media, KC_MUTE),
  COMBO(thumbcombos_num, KC_DOT),
  #if defined (MIRYOKU_LAYERS_FLIP)
  COMBO(thumbcombos_sym, KC_RPRN),
  #else
  COMBO(thumbcombos_sym, KC_LPRN),
  #endif
  COMBO(thumbcombos_fun, KC_APP)
};
#endif
