// TODO - Copyright

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


// Qmk definitions

// Required by RGB_MATRIX_DEFAULT_VAL
// Duplicated from rgb_matrix.c
#if !defined(RGB_MATRIX_MAXIMUM_BRIGHTNESS) || RGB_MATRIX_MAXIMUM_BRIGHTNESS > UINT8_MAX
#  undef RGB_MATRIX_MAXIMUM_BRIGHTNESS
#  define RGB_MATRIX_MAXIMUM_BRIGHTNESS UINT8_MAX
#endif


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


// Persistent user state

typedef union {
  uint32_t raw;
  struct {
    // For mac mode look for ctl-gui-swap
    bool os_mode_linux :1;
  };
} user_config_t;

user_config_t user_config;


// Modifier functions

#define LAYER_MASK_NUM (1 << U_NUM)

// Defined in manna-harbour_miryoku.c
extern const key_override_t capsword_key_override;
extern const key_override_t **key_overrides;

const key_override_t dot_key_override = ko_make_with_layers(MOD_MASK_SHIFT, KC_DOT, KC_LEFT_PAREN, LAYER_MASK_NUM);
// const key_override_t nine_key_override = ko_make_with_layers(MOD_MASK_SHIFT, KC_9, U_USER, LAYER_MASK_NUM);
const key_override_t nine_key_override = ko_make_with_layers_negmods_and_options(
  MOD_MASK_SHIFT, KC_9, U_USER, LAYER_MASK_NUM, 0, ko_option_no_unregister_on_other_key_down);


const key_override_t **custom_key_overrides = (const key_override_t *[]){
  &capsword_key_override,
  &dot_key_override,
  &nine_key_override,
  NULL
};


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


// OS-specific clipboard and undo/redo

typedef enum {
  CLIP_CUT,
  CLIP_CPY,
  CLIP_PST,
  CLIP_UND,
  CLIP_RDO,
  CLIP_END,
} clip_t;

// Windows os keycodes (without ctrl)
const uint16_t PROGMEM os_win_keycodes[] = { KC_X, KC_C, KC_V, KC_Z, KC_Y, };
// Mac os keycodes
const uint16_t PROGMEM os_mac_keycodes[] = { LCMD(KC_X), LCMD(KC_C), LCMD(KC_V), LCMD(KC_Z), SCMD(KC_Z), };
// Linux os keycodes are left as-is

bool process_clipcode(clip_t clip, keyrecord_t *record) {
  if (os_mode == OS_MODE_WIN) {
    // Windows keycodes are translated
    if (record->event.pressed) {
      // Right control avoids interaction with Miryoku left control
      register_code16(KC_RCTL);
      // Add delay for Remote Desktop
      wait_ms(TAP_CODE_DELAY);
      register_code16(os_win_keycodes[clip]);
    } else {
      unregister_code16(os_win_keycodes[clip]);
      unregister_code16(KC_RCTL);
    }
    return false;
  }
  
  if (os_mode == OS_MODE_MAC) {
    // Mac keycodes are translated
    if (record->event.pressed)
      register_code16(os_mac_keycodes[clip]);
    else
      unregister_code16(os_mac_keycodes[clip]);
    return false;
  }
  
  // Linux keycodes are passed through as-is
  return true;
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
  uint8_t layer = get_highest_layer(state|default_layer_state);
  show_layer(layer);
  return state;
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
  uint8_t default_layer = get_highest_layer(state);
  show_default_layer(default_layer);
  uint8_t layer = get_highest_layer(state|layer_state);
  show_layer(layer);
  return state;
}


// Rgb

#ifdef RGB_MATRIX_ENABLE

// Override keys to allow feedback on keydown
// (A post_process_record_user implementation is simpler, but fails)
// (The user function is called on keyup but not on keydown following a return-false)

// Return true if stepped value i (qadd8, qsub8) is on detent
bool slider_on_detent(uint8_t i, uint8_t detent, int8_t step) {
  uint8_t lower = detent - (step / 2);
  uint8_t upper = lower + step - 1;
  // Wrap around? Floor at 0
  if (lower > detent) lower = 0;
  // Wrap around? Cap at 0xFF
  if (upper < detent) upper = 255;
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
  
  uint8_t shifted = get_mods() & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_step();
  else
    rgb_matrix_step_reverse();
  
  uint8_t mode = rgb_matrix_get_mode();
  bool detent = (mode == RGB_MATRIX_DEFAULT_MODE);
  show_value(RGB_MOD, mode, detent);
  return false;
}

bool process_rgb_hue(keyrecord_t *record) {
  if (!record->event.pressed)
    return false;
  
  uint8_t shifted = get_mods() & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_increase_hue();
  else
    rgb_matrix_decrease_hue();
  
  uint8_t hue = rgb_matrix_get_hue();
  bool detent = (hue == RGB_MATRIX_DEFAULT_HUE);
  show_value(RGB_HUI, hue, detent);
  return false;
}

bool process_rgb_sat(keyrecord_t *record) {
  if (!record->event.pressed)
    return false;
  
  uint8_t shifted = get_mods() & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_increase_sat();
  else
    rgb_matrix_decrease_sat();
  
  uint8_t sat = rgb_matrix_get_sat();
  bool detent = slider_on_detent(sat, RGB_MATRIX_DEFAULT_SAT, RGB_MATRIX_SAT_STEP);
  show_value(RGB_SAI, sat, detent);
  return false;
}

bool process_rgb_val(keyrecord_t *record) {
  if (!record->event.pressed)
    return false;
  
  uint8_t shifted = get_mods() & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_increase_val();
  else
    rgb_matrix_decrease_val();
  
  uint8_t val = rgb_matrix_get_val();
  bool detent = slider_on_detent(val, RGB_MATRIX_DEFAULT_VAL, RGB_MATRIX_VAL_STEP);
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
  
  uint8_t spd = rgb_matrix_get_speed();
  bool detent = slider_on_detent(spd, RGB_MATRIX_DEFAULT_SPD, RGB_MATRIX_SPD_STEP);
  show_value(RGB_SPI, spd, detent);
  return false;
}

#endif


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


// Leader sequences

#ifdef LEADER_ENABLE

LEADER_EXTERNS();

void matrix_scan_user(void) {
  LEADER_DICTIONARY() {
    leading = false;
    leader_end();

    // Placeholder for leader key sequences
  }
}

#endif

// Key processing

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
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
    case U_USER:
      // Placeholder
      return false;
    default:
      return true;
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
