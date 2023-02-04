// Copyright 2022 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


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


// shift functions

#if 0

const key_override_t capsword_key_override = ko_make_basic(MOD_MASK_SHIFT, CW_TOGG, KC_CAPS);

const key_override_t **key_overrides = (const key_override_t *[]){
    &capsword_key_override,
    NULL
};

-- my stuff here

#define LAYER_MASK_NUM (1 << U_NUM)
#define LAYER_MASK_SYM (1 << U_SYM)
#define LAYER_MASK_MEDIA (1 << U_MEDIA)

const key_override_t capsword_key_override = ko_make_basic(MOD_MASK_SHIFT, CW_TOGG, KC_CAPS);

const key_override_t to_left_paren_num_key_override = ko_make_with_layers(MOD_MASK_SHIFT, KC_DOT, KC_LEFT_PAREN, LAYER_MASK_NUM);
const key_override_t to_lead_num_key_override = ko_make_with_layers(MOD_MASK_SHIFT, KC_9, KC_LEAD, LAYER_MASK_NUM);
const key_override_t to_lead_sym_key_override = ko_make_with_layers(0, KC_LEFT_PAREN, KC_LEAD, LAYER_MASK_SYM);

const key_override_t to_audio_toggle_media_key_override = ko_make_with_layers(MOD_MASK_CTRL, RGB_TOG, QK_AUDIO_TOGGLE, LAYER_MASK_MEDIA);
const key_override_t to_rgb_speed_increase_media_key_override = ko_make_with_layers(MOD_MASK_CTRL, RGB_SAI, RGB_SPI, LAYER_MASK_MEDIA);


const key_override_t **key_overrides = (const key_override_t *[]){
    &capsword_key_override,
    &to_left_paren_num_key_override,
    &to_lead_num_key_override,
    &to_lead_sym_key_override,
    &to_audio_toggle_media_key_override,
    &to_rgb_speed_increase_media_key_override,
    NULL
};

#else

const key_override_t capsword_key_override = ko_make_basic(MOD_MASK_SHIFT, CW_TOGG, KC_CAPS);

const key_override_t **key_overrides = (const key_override_t *[]){
    &capsword_key_override,
    NULL
};

const key_override_t capsword_key_override = ko_make_basic(MOD_MASK_SHIFT, CW_TOGG, KC_CAPS);

const key_override_t ko1 = ko_make_basic(MOD_MASK_SHIFT, KC_Q, RGB_SPI);
const key_override_t ko2 = ko_make_basic(MOD_MASK_SHIFT, KC_W, RGB_SPD);
const key_override_t ko3 = ko_make_basic(0, KC_A, RGB_SPI);
const key_override_t ko4 = ko_make_basic(0, KC_R, RGB_SPD);
const key_override_t ko5 = ko_make_with_layers(0, KC_Z, RGB_SPI, ~0);
const key_override_t ko6 = ko_make_with_layers(0, KC_X, RGB_SPD, ~0);
const key_override_t ko7 = ko_make_with_layers(0, KC_K, RGB_SPI, 1);
const key_override_t ko8 = ko_make_with_layers(0, KC_H, RGB_SPD, 1);

const key_override_t **key_overrides = (const key_override_t *[]){
    &capsword_key_override,
    &ko1,
    &ko2,
    &ko3,
    &ko4,
    &ko5,
    &ko6,
    &ko7,
    &ko8,
    NULL
};

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
