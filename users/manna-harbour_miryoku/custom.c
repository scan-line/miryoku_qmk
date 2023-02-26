// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


// Qmk definitions

// Required by RGB_MATRIX_DEFAULT_VAL
// Duplicated from rgb_matrix.c
#if !defined(RGB_MATRIX_MAXIMUM_BRIGHTNESS) || RGB_MATRIX_MAXIMUM_BRIGHTNESS > UINT8_MAX
#  undef RGB_MATRIX_MAXIMUM_BRIGHTNESS
#  define RGB_MATRIX_MAXIMUM_BRIGHTNESS UINT8_MAX
#endif


// Persistent user state

typedef union {
  uint32_t raw;
  struct {
    // Set if in linux mode
    // See also ctl-gui-swap for mac mode / windows mode
    bool os_mode_linux :1;
  };
} user_config_t;

user_config_t user_config;


// Sounds

#ifdef AUDIO_ENABLE

float mode_set_song[][2] = MODE_SET_SONG;
float layer_set_song[][2] = LAYER_SET_SONG;
float toggle_on_song[][2] = TOGGLE_ON_SONG;
float toggle_off_song[][2] = TOGGLE_OFF_SONG;
float detent_song[][2] = DETENT_SONG;

#else

#define PLAY_SONG(song)

#endif


// Feedback

__attribute__((weak)) void show_mode_custom(uint16_t keycode) {
}

void show_mode(uint16_t keycode) {
  PLAY_SONG(mode_set_song);
  show_mode_custom(keycode);
}

__attribute__((weak)) void show_layer_custom(uint8_t layer) {
}

void show_layer(uint8_t layer) {
  show_layer_custom(layer);
}

__attribute__((weak)) void show_default_layer_custom(uint8_t layer) {
}

void show_default_layer(uint8_t layer) {
  PLAY_SONG(layer_set_song);
  show_default_layer_custom(layer);
}

__attribute__((weak)) void show_toggle_custom(uint16_t keycode, bool value) {
}

void show_toggle(uint16_t keycode, bool value) {
  if (value)
    PLAY_SONG(toggle_on_song);
  else
    PLAY_SONG(toggle_off_song);
  show_toggle_custom(keycode, value);
}

__attribute__((weak)) void show_value_custom(uint16_t keycode, uint16_t value, bool detent) {
}

void show_value(uint16_t keycode, uint16_t value, bool detent) {
  if (detent)
    PLAY_SONG(detent_song);
  show_value_custom(keycode, value, detent);
}


// Layers

const char* layer_name(uint8_t layer) {
  switch(layer) {
#   define MIRYOKU_X(LAYER, STRING) case U_##LAYER: return STRING;
      MIRYOKU_LAYER_LIST
#   undef MIRYOKU_X
    default:
      return "Unknown";
  }
}

layer_state_t layer_state_set_user(layer_state_t state) {
  const uint8_t layer = get_highest_layer(state|default_layer_state);
  show_layer(layer);
  return state;
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
  const uint8_t default_layer = get_highest_layer(state);
  show_default_layer(default_layer);
  const uint8_t layer = get_highest_layer(state|layer_state);
  show_layer(layer);
  return state;
}


// OS-specific mode

typedef enum {
  OS_MODE_WIN,
  OS_MODE_MAC,
  OS_MODE_LNX,
} os_mode_t;

os_mode_t os_mode = OS_MODE_WIN;

os_mode_t os_mode_get(void) {
  if (user_config.os_mode_linux)
    return OS_MODE_LNX;
  else if (keymap_config.swap_lctl_lgui)
    return OS_MODE_MAC;
  else
    return OS_MODE_WIN;
}

bool process_os_mode(os_mode_t mode, keyrecord_t *record) {
  if (!record->event.pressed)
    return false;
  
  os_mode = mode;
  
  user_config.os_mode_linux = (mode == OS_MODE_LNX);
  eeconfig_update_user(user_config.raw);
  
  if (mode == OS_MODE_MAC)
    process_magic(MAGIC_SWAP_CTL_GUI, record);
  else
    process_magic(MAGIC_UNSWAP_CTL_GUI, record);
  
  switch(mode) {
    case OS_MODE_WIN:
      show_mode(U_WIN);
      break;
    case OS_MODE_MAC:
      show_mode(U_MAC);
      break;
    case OS_MODE_LNX:
      show_mode(U_LNX);
      break;
    default:
      break;
  }
  return false;
}


// User key

// UK pound macros
#define OS_WIN_GBP  \
  SS_DOWN(X_LALT)   \
  SS_TAP(X_KP_0)    \
  SS_TAP(X_KP_1)    \
  SS_TAP(X_KP_6)    \
  SS_TAP(X_KP_3)    \
  SS_UP(X_LALT)
#define OS_MAC_GBP SS_RALT("3")
#define OS_LNX_GBP SS_LCTL("U") "00A3" SS_TAP(X_ENTER)

void register_user() {
  // Send UK pound
  // (no autorepeat)
  switch (os_mode) {
    case OS_MODE_WIN;
      SEND_STRING_DELAY(OS_WIN_GBP, TAP_CODE_DELAY);
      break;
    case OS_MODE_MAC;
      SEND_STRING_DELAY(OS_MAC_GBP, TAP_CODE_DELAY);
      break;
    case OS_MODE_LNX;
      SEND_STRING_DELAY(OS_LNX_GBP, TAP_CODE_DELAY);
      break;
    default:
      break;
  }
}

void unregister_user() {
}

void tap_user() {
  register_user();
  unregister_user();
}

bool process_user(keyrecord_t *record) {
  if (record->event.pressed)
    register_user();
  else
    unregister_user();
  return false;
}


// OS-specific clipboard and undo/redo

typedef enum {
  CLIP_CUT,
  CLIP_CPY,
  CLIP_PST,
  CLIP_UND,
  CLIP_RDO,
  CLIP_END,
} clip_t;

// Windows os keycodes
const uint16_t PROGMEM os_win_keycodes[] = { LCTL(KC_X), LCTL(KC_C), LCTL(KC_V), LCTL(KC_Z), LCTL(KC_Y), };
// Mac os keycodes
const uint16_t PROGMEM os_mac_keycodes[] = { LCMD(KC_X), LCMD(KC_C), LCMD(KC_V), LCMD(KC_Z), SCMD(KC_Z), };
// Linux os keycodes are left as-is

bool process_clipcode(clip_t clip, keyrecord_t *record) {
  // Windows keycodes are translated
  if (os_mode == OS_MODE_WIN) {
    const uint16_t keycode = os_win_keycodes[clip];
    const uint8_t mods = QK_MODS_GET_MODS(keycode);
    if (record->event.pressed) {
      register_weak_mods(mods);
      // Delay between mods and key down for Windows Remote Desktop
      wait_ms(TAP_CODE_DELAY);
      register_code(keycode);
    } else {
      unregister_code(keycode);
      unregister_weak_mods(mods);
    }
    return false;
  }
  
  // Mac keycodes are translated
  if (os_mode == OS_MODE_MAC) {
    const uint16_t keycode = os_mac_keycodes[clip];
    if (record->event.pressed)
      register_code16(keycode);
    else
      unregister_code16(keycode);
    return false;
  }
  
  // Linux keycodes are passed through as-is
  return true;
}


// Audio

#ifdef AUDIO_ENABLE

bool process_audio_toggle(keyrecord_t *record) {
  if (!record->event.pressed)
    return false;
  
  // Show toggle in the on state
  if (audio_is_on()) {
      show_toggle(QK_AUDIO_TOGGLE, false);
      // Wait for the show_toggle sound to finish
      // (the wait inside audio_off is not enough)
      wait_ms(100);
      audio_off();
  } else {
      audio_on();
      show_toggle(QK_AUDIO_TOGGLE, true);
  }
  return false;
}

#endif


// Rgb

#ifdef RGB_MATRIX_ENABLE

// Override keys to allow feedback on keydown
// (A post_process_record_user implementation is simpler, but fails)
// (The user function is called on keyup but not on keydown following a return-false)

// Return true if stepped value i (qadd8, qsub8) is on detent
bool slider_on_detent(uint8_t i, uint8_t detent, int8_t step) {
  uint8_t lower = detent - (step / 2);
  uint8_t upper = lower + step - 1;
  // Wrap around? floor at 0
  if (lower > detent)
    lower = 0;
  // Wrap around? cap at 0xFF
  if (upper < detent)
    upper = 255;
  return (lower <= i && i <= upper);
}

bool process_rgb_toggle(keyrecord_t *record) {
  if (!record->event.pressed)
    return false;
  
  // Show toggle in the on state
  if (rgb_matrix_is_enabled()) {
    show_toggle(RGB_TOG, false);
    rgb_matrix_disable();
  } else {
    rgb_matrix_enable();
    show_toggle(RGB_TOG, true);
  }
  return false;
}

bool process_rgb_mode(keyrecord_t *record) {
  if (!record->event.pressed)
    return false;
  
  const uint8_t shifted = get_mods() & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_step();
  else
    rgb_matrix_step_reverse();
  
  const uint8_t mode = rgb_matrix_get_mode();
  const bool detent = (mode == RGB_MATRIX_DEFAULT_MODE);
  show_value(RGB_MOD, mode, detent);
  return false;
}

bool process_rgb_hue(keyrecord_t *record) {
  if (!record->event.pressed)
    return false;
  
  const uint8_t shifted = get_mods() & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_increase_hue();
  else
    rgb_matrix_decrease_hue();
  
  const uint8_t hue = rgb_matrix_get_hue();
  const bool detent = (hue == RGB_MATRIX_DEFAULT_HUE);
  show_value(RGB_HUI, hue, detent);
  return false;
}

bool process_rgb_sat(keyrecord_t *record) {
  if (!record->event.pressed)
    return false;
  
  const uint8_t shifted = get_mods() & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_increase_sat();
  else
    rgb_matrix_decrease_sat();
  
  const uint8_t sat = rgb_matrix_get_sat();
  const bool detent = slider_on_detent(sat, RGB_MATRIX_DEFAULT_SAT, RGB_MATRIX_SAT_STEP);
  show_value(RGB_SAI, sat, detent);
  return false;
}

bool process_rgb_val(keyrecord_t *record) {
  if (!record->event.pressed)
    return false;
  
  const uint8_t shifted = get_mods() & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_increase_val();
  else
    rgb_matrix_decrease_val();
  
  const uint8_t val = rgb_matrix_get_val();
  const bool detent = slider_on_detent(val, RGB_MATRIX_DEFAULT_VAL, RGB_MATRIX_VAL_STEP);
  show_value(RGB_VAI, val, detent);
  return false;
}

bool process_rgb_speed(keyrecord_t *record) {
  if (!record->event.pressed)
    return false;
  
  uint8_t shifted = get_mods() & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_increase_speed();
  else
    rgb_matrix_decrease_speed();
  
  const uint8_t spd = rgb_matrix_get_speed();
  const bool detent = slider_on_detent(spd, RGB_MATRIX_DEFAULT_SPD, RGB_MATRIX_SPD_STEP);
  show_value(RGB_SPI, spd, detent);
  return false;
}

#endif


// Key processing

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case U_USER:
      return process_user(record);
    case U_WIN:
      return process_os_mode(OS_MODE_WIN, record);
    case U_MAC:
      return process_os_mode(OS_MODE_MAC, record);
    case U_LNX:
      return process_os_mode(OS_MODE_LNX, record);
    case U_CUT:
      return process_clipcode(CLIP_CUT, record);
    case U_CPY:
      return process_clipcode(CLIP_CPY, record);
    case U_PST:
      return process_clipcode(CLIP_PST, record);
    case U_UND:
      return process_clipcode(CLIP_UND, record);
    case U_RDO:
      return process_clipcode(CLIP_RDO, record);
#ifdef RGB_MATRIX_ENABLE
    case U_RGB_TOG:
      return process_rgb_toggle(record);
    case U_RGB_MOD:
      return process_rgb_mode(record);
    case U_RGB_HUI:
      return process_rgb_hue(record);
    case U_RGB_SAI:
      return process_rgb_sat(record);
    case U_RGB_VAI:
      return process_rgb_val(record);
    case U_RGB_SPI:
      return process_rgb_speed(record);
#endif
#ifdef AUDIO_ENABLE
    case U_AUD_TOG:
      return process_audio_toggle(record);
#endif
    default:
      return true;
  }
}


// Shift and Auto Shift overrides

#define LAYER_MASK_NUM (1 << U_NUM)

// Defined in action_util.c
extern void set_weak_override_mods(uint8_t mods);
// Defined in manna-harbour_miryoku.c
extern const key_override_t capsword_key_override;
extern const key_override_t **key_overrides;

bool key_override_tap(bool key_down, void *context) {
  uint16_t keycode = (intptr_t)context;
  
  // Tap to prevent autorepeat
  if (key_down) {
    uint8_t mods = QK_MODS_GET_MODS(keycode);
    set_weak_override_mods(mods);
    
    if (keycode == U_USER)
      tap_user();
    else
      tap_code(keycode);
  }
  return false;
}

// Customized ko_make_with_layers
// Removes auto-repeat on keypress
#define user_ko_make(trigger_mods_, trigger_key, replacement_key, layer_mask)  \
  ((const key_override_t){                                    \
    .trigger_mods       = (trigger_mods_),                    \
    .layers             = (layer_mask),                       \
    .suppressed_mods    = (trigger_mods_),                    \
    .options            = ko_options_default,                 \
    .negative_mod_mask  = 0,                                  \
    .custom_action      = key_override_tap,                   \
    .context            = (void*)(intptr_t)replacement_key,   \
    .trigger            = (trigger_key),                      \
    .replacement        = (KC_NO),                            \
    .enabled            = NULL                                \
  })

const key_override_t dot_key_override = user_ko_make(MOD_MASK_SHIFT, KC_DOT, KC_LEFT_PAREN, LAYER_MASK_NUM );
const key_override_t nine_key_override = user_ko_make(MOD_MASK_SHIFT, KC_9, U_USER, LAYER_MASK_NUM );

const key_override_t **custom_key_overrides = (const key_override_t *[]){
  &capsword_key_override,
  &dot_key_override,
  &nine_key_override,
  NULL
};

bool get_custom_auto_shifted_key(uint16_t keycode, keyrecord_t *record) {
  const uint8_t layer = read_source_layers_cache(record->event.key);

  // Add, even where auto-shifted by default
  // For consistency with autoshift_press/release_user
  
  if (keycode == KC_DOT && layer == U_NUM)
    return true;
  
  if (keycode == KC_9 && layer == U_NUM)
    return true;

  return false;
}

void autoshift_press_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
  const uint8_t layer = read_source_layers_cache(record->event.key);
  // Beware - record->event.pressed may be false

  if (keycode == KC_DOT && layer == U_NUM) {
    register_code16((!shifted) ? KC_DOT : KC_LEFT_PAREN);
    return;
  }
  
  if (keycode == KC_9 && layer == U_NUM) {
    if (!shifted)
      register_code16(KC_9)
    else
      register_user();
    return;
  }

  if (shifted)
    add_weak_mods(MOD_BIT(KC_LSFT));
  // & 0xFF gets the Tap key for Tap Holds, required when using Retro Shift
  register_code16((IS_RETRO(keycode)) ? keycode & 0xFF : keycode);
}

void autoshift_release_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
  const uint8_t layer = read_source_layers_cache(record->event.key);

  if (keycode == KC_DOT && layer == U_NUM) {
    unregister_code16((!shifted) ? KC_DOT : KC_LEFT_PAREN);
    return;
  }
  
  if (keycode == KC_9 && layer == U_NUM) {
    if (!shifted)
      unregister_code16(KC_9);
    else
      unregister_user();
    return;
  }

  // & 0xFF gets the Tap key for Tap Holds, required when using Retro Shift
  // The IS_RETRO check isn't really necessary here, always using
  // keycode & 0xFF would be fine.
  unregister_code16((IS_RETRO(keycode)) ? keycode & 0xFF : keycode);
  // Clearing mods is handled by caller
}


// Tapping term

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
  // Extra delay on weaker ring and pinky fingers
  switch (keycode) {
    // Colemak DH, Qwerty
    case LGUI_T(KC_A):
    case LGUI_T(KC_O):
    case LGUI_T(KC_QUOT):
    case LALT_T(KC_R):
    case LALT_T(KC_I):
    case LALT_T(KC_S):
    case LALT_T(KC_L):
      return SLOW_TAPPING_TERM;
    default:
      return FAST_TAPPING_TERM;
  }
}


// Initialization

__attribute__((weak)) void eeconfig_init_custom(void) {
}

void eeconfig_init_user(void) {
  user_config.raw = 0;
  // Windows mode by default
  // keymap_config.swap_lctl_lgui defaults to false
  user_config.os_mode_linux = false;
  
  eeconfig_update_user(user_config.raw);
  eeconfig_init_custom();
}

void keyboard_post_init_user(void) {
  // Patch key overrides with our extended list
  key_overrides = custom_key_overrides;
  
  // Restore user state
  user_config.raw = eeconfig_read_user();
  os_mode = os_mode_get();
}
