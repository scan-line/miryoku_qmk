// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once


// Keycodes

// Defining an enum here (as recommended in qmk docs) does not work
// This header is included by both c and assembler source
// and the assembly will fail
// Steal some unused keycodes instead

#define U_USER PB_1
#define U_WIN PB_2
#define U_MAC PB_3
#define U_LNX PB_4

#define U_RGB_TOG PB_5
#define U_RGB_MOD PB_6
#define U_RGB_HUI PB_7
#define U_RGB_SAI PB_8
#define U_RGB_VAI PB_9
#define U_RGB_SPI PB_10
#define U_AUD_TOG PB_11


// Layers

#define MIRYOKU_LAYER_MEDIA \
TD(U_TD_BOOT),     TD(U_TD_U_TAP),    TD(U_TD_U_EXTRA),  TD(U_TD_U_BASE),   U_NA,              U_RGB_TOG,         U_RGB_MOD,         U_RGB_HUI,         U_RGB_SAI,         U_RGB_VAI,         \
KC_LGUI,           KC_LALT,           KC_LCTL,           KC_LSFT,           U_NA,              U_AUD_TOG,         KC_MPRV,           KC_VOLD,           KC_VOLU,           KC_MNXT,           \
U_NA,              KC_ALGR,           TD(U_TD_U_FUN),    TD(U_TD_U_MEDIA),  U_NA,              OU_AUTO,           U_WIN,             U_MAC,             U_LNX,             U_RGB_SPI,         \
U_NP,              U_NP,              U_NA,              U_NA,              U_NA,              KC_MSTP,           KC_MPLY,           KC_MUTE,           U_NP,              U_NP

#define MIRYOKU_LAYER_SYM \
KC_LCBR,           KC_AMPR,           KC_ASTR,           U_USER,            KC_RCBR,           U_NA,              TD(U_TD_U_BASE),   TD(U_TD_U_EXTRA),  TD(U_TD_U_TAP),    TD(U_TD_BOOT),     \
KC_COLN,           KC_DLR,            KC_PERC,           KC_CIRC,           KC_PLUS,           U_NA,              KC_LSFT,           KC_LCTL,           KC_LALT,           KC_LGUI,           \
KC_TILD,           KC_EXLM,           KC_AT,             KC_HASH,           KC_PIPE,           U_NA,              TD(U_TD_U_SYM),    TD(U_TD_U_MOUSE),  KC_ALGR,           U_NA,              \
U_NP,              U_NP,              KC_LPRN,           KC_RPRN,           KC_UNDS,           U_NA,              U_NA,              U_NA,              U_NP,              U_NP


// Sound design

#ifdef AUDIO_ENABLE

#define MODE_SET_SONG SONG(AG_NORM_SOUND)
#define LAYER_SET_SONG SONG(AG_SWAP_SOUND)
#define TOGGLE_ON_SONG SONG(AUDIO_ON_SOUND)
#define TOGGLE_OFF_SONG SONG(AUDIO_OFF_SOUND)
#define DETENT_SONG SONG(Q__NOTE(_A4),)

#define AUDIO_INIT_DELAY

#define AUDIO_ON_SONG SONG(NO_SOUND)
#define AUDIO_OFF_SONG SONG(NO_SOUND)
#define CG_NORM_SONG SONG(NO_SOUND)
#define CG_SWAP_SONG SONG(NO_SOUND)

#endif


// Lighting design

#ifdef RGBLIGHT_ENABLE
#  define RGBLIGHT_DISABLE_KEYCODES
#endif

#ifdef RGB_MATRIX_ENABLE
#  define RGB_MATRIX_DISABLE_KEYCODES
#endif


// Custom tap-dance implementation

#ifndef __ASSEMBLER__

// Keep tap-dance keycodes
#define TD(N) (QK_TAP_DANCE | TD_INDEX(N))
#define TD_INDEX(CODE) ((CODE)&0xFF)

typedef struct {
  uint8_t count;
} double_tap_state_t;

typedef void (*double_tap_action_t)(qk_tap_dance_state_t *state, void *unused);

// Replacements for manna-harbour_miryoku.c
#define qk_tap_dance_state_t double_tap_state_t
#define qk_tap_dance_action_t double_tap_action_t
#define ACTION_TAP_DANCE_FN(FUNCTION) FUNCTION

#endif


// Custom key-override implementation

#ifndef __ASSEMBLER__

typedef struct {
  uint16_t trigger;
  uint16_t replacement;
  layer_state_t layers;
} shift_override_t;

#define make_shift_override(TRIGGER, REPLACEMENT, LAYERS) \
  ((const shift_override_t){        \
    .trigger = (TRIGGER),           \
    .replacement = (REPLACEMENT),   \
    .layers = (LAYERS)              \
  })

// Replacements for manna-harbour_miryoku.c
#define key_override_t shift_override_t
#define ko_make_basic(MASK, TRIGGER, REPLACEMENT) \
  make_shift_override(TRIGGER, REPLACEMENT, ~0)

#endif


// Configure QMK

// Beware - Miryoku config.h unconditionally resets TAPPING_TERM to 200
// Per-key tapping terms
#define TAPPING_TERM_PER_KEY
#define FAST_TAPPING_TERM (TAPPING_TERM + 30)
#define SLOW_TAPPING_TERM (TAPPING_TERM + 100)
