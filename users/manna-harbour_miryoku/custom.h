// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <stdint.h>

#include "action_layer.h"
#include "keycodes.h"


// Keycodes

enum my_keycodes {
  U_USER = SAFE_RANGE,
  U_WIN,
  U_MAC,
  U_LNX,
  U_RGB_TOG,
  U_RGB_MOD,
  U_RGB_HUI,
  U_RGB_SAI,
  U_RGB_VAI,
  U_RGB_SPI,
  U_AUD_TOG,
};


// Custom double-tap implementation

#define DT(N) (QK_TAP_DANCE | DT_INDEX(N))
#define DT_INDEX(CODE) ((CODE)&0xFF)
#define IS_QK_DOUBLE_TAP IS_QK_TAP_DANCE

typedef struct {
  uint8_t count;
} double_tap_state_t;

typedef void (*double_tap_function_t)(double_tap_state_t *state, void *reserved);

#define TD(N) DT(N)
#define TD_INDEX(CODE) DT_INDEX(CODE)
#define qk_tap_dance_state_t double_tap_state_t
#define qk_tap_dance_action_t double_tap_action_t
#define ACTION_TAP_DANCE_FN(FUNCTION) FUNCTION


// Custom key-override implementation

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

#define key_override_t shift_override_t
#define ko_make_basic(MASK, TRIGGER, REPLACEMENT) \
  make_shift_override(TRIGGER, REPLACEMENT, ~0)
