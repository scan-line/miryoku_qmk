// TODO - Copyright

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


// Qmk definitions

// Duplicated from rgb_matrix.c
#if !defined(RGB_MATRIX_MAXIMUM_BRIGHTNESS) || RGB_MATRIX_MAXIMUM_BRIGHTNESS > UINT8_MAX
#    undef RGB_MATRIX_MAXIMUM_BRIGHTNESS
#    define RGB_MATRIX_MAXIMUM_BRIGHTNESS UINT8_MAX
#endif


// Sounds

float mode_set_song[][2] = MODE_SET_SONG;
float layer_set_song[][2] = LAYER_SET_SONG;
float toggle_on_song[][2] = TOGGLE_ON_SONG;
float toggle_off_song[][2] = TOGGLE_OFF_SONG;
float detent_song[][2] = DETENT_SONG;


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
const key_override_t nine_key_override = ko_make_with_layers(MOD_MASK_SHIFT, KC_9, KC_LEAD, LAYER_MASK_NUM);

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
  if (user_config.os_mode_linux) {
    return OS_MODE_LNX;
  } else if (keymap_config.swap_lctl_lgui) {
    return OS_MODE_MAC;
  } else {
    return OS_MODE_WIN;
  }
}

bool process_os_mode(os_mode_t mode, keyrecord_t *record) {
  if (record->event.pressed) {
    os_mode = mode;
  
    user_config.os_mode_linux = (mode == OS_MODE_LNX);
    eeconfig_update_user(user_config.raw);
  
    if (mode == OS_MODE_MAC) {
      process_magic(MAGIC_SWAP_CTL_GUI, record);
    } else {
      process_magic(MAGIC_UNSWAP_CTL_GUI, record);
    }
    
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
  }
  return false;
}


// OS-specific clipboard and undo/redo

// Windows clipboard
#define CLIP_CUT_WIN LCTL(KC_X)
#define CLIP_CPY_WIN LCTL(KC_C)
#define CLIP_PST_WIN LCTL(KC_V)
#define CLIP_UND_WIN LCTL(KC_Z)
#define CLIP_RDO_WIN LCTL(KC_Y)

// Mac clipboard
#define CLIP_CUT_MAC LCMD(KC_X)
#define CLIP_CPY_MAC LCMD(KC_C)
#define CLIP_PST_MAC LCMD(KC_V)
#define CLIP_UND_MAC LCMD(KC_Z)
#define CLIP_RDO_MAC SCMD(KC_Z)

typedef enum {
  CLIP_CUT,
  CLIP_CPY,
  CLIP_PST,
  CLIP_UND,
  CLIP_RDO,
  CLIP_END,
} clip_t;

// Windows and Mac only
// Linux keycodes are left as-is
const uint16_t PROGMEM os_keycodes[][CLIP_END] = {
  [OS_MODE_WIN] = { CLIP_CUT_WIN, CLIP_CPY_WIN, CLIP_PST_WIN, CLIP_UND_WIN, CLIP_RDO_WIN, },
  [OS_MODE_MAC] = { CLIP_CUT_MAC, CLIP_CPY_MAC, CLIP_PST_MAC, CLIP_UND_MAC, CLIP_RDO_MAC, },
};

bool process_clipcode(clip_t clip, keyrecord_t *record) {
  // Linux keycodes are passed through as-is
  if (os_mode == OS_MODE_LNX) {
    return true;
  }
  // Windows and Mac keycodes are translated
  if (record->event.pressed) {
    register_code16(os_keycodes[os_mode][clip]);
  } else {
    unregister_code16(os_keycodes[os_mode][clip]);
  }
  return false;
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

// Override keys to allow feedback on keydown
// (A post_process_record_user implementation is simpler, but fails)
// (The user function is called on keyup but not on keydown following a return-false)

// Return true if stepped value i (qadd8, qsub8) is on target
bool slider_on_target(uint8_t i, uint8_t target, int8_t step) {
  uint8_t lower = target - (step / 2);
  uint8_t upper = lower + step - 1;
  // Wrap around? Floor at 0
  if (lower > target) lower = 0;
  // Wrap around? Cap at 0xFF
  if (upper < target) upper = 255;
  return (lower <= i && i <= upper);
}

bool process_rgb_toggle(keyrecord_t *record) {
  if (!record->event.pressed)
      return true;
  
  rgb_matrix_toggle();
  show_toggle(RGB_TOG, rgb_matrix_is_enabled());
  return false;
}

bool process_rgb_mode(keyrecord_t *record) {
  if (!record->event.pressed)
      return true;
  
  uint8_t shifted = get_mods() & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_step();
  else
    rgb_matrix_step_reverse();
  
  if (rgb_matrix_get_mode() == RGB_MATRIX_DEFAULT_MODE)
    PLAY_SONG(detent_song);
  return false;
}

bool process_rgb_hue(keyrecord_t *record) {
  if (!record->event.pressed)
      return true;
  
  uint8_t shifted = get_mods() & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_increase_hue();
  else
    rgb_matrix_decrease_hue();
  
  if (rgb_matrix_get_hue() == RGB_MATRIX_DEFAULT_HUE)
    PLAY_SONG(detent_song);
  return false;
}

bool process_rgb_sat(keyrecord_t *record) {
  if (!record->event.pressed)
      return true;
  
  uint8_t shifted = get_mods() & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_increase_sat();
  else
    rgb_matrix_decrease_sat();
  
  if (slider_on_target(rgb_matrix_get_sat(), RGB_MATRIX_DEFAULT_SAT, RGB_MATRIX_SAT_STEP))
    PLAY_SONG(detent_song);
  return false;
}

bool process_rgb_val(keyrecord_t *record) {
  if (!record->event.pressed)
      return true;
  
  uint8_t shifted = get_mods() & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_increase_val();
  else
    rgb_matrix_decrease_val();
  
  if (slider_on_target(rgb_matrix_get_val(), RGB_MATRIX_DEFAULT_VAL, RGB_MATRIX_VAL_STEP))
    PLAY_SONG(detent_song);
  return false;
}

bool process_rgb_speed(keyrecord_t *record) {
  if (!record->event.pressed)
      return true;
  
  uint8_t shifted = get_mods() & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_increase_speed();
  else
    rgb_matrix_decrease_speed();
  
  if (slider_on_target(rgb_matrix_get_speed(), RGB_MATRIX_DEFAULT_SPD, RGB_MATRIX_SPD_STEP))
    PLAY_SONG(detent_song);
  return false;
}


// Audio

bool process_aud_toggle(keyrecord_t *record) {
  if (!record->event.pressed)
      return true;
  
    if (is_audio_on()) {
        show_toggle(QK_AUDIO_TOGGLE, false);
        audio_off();
    } else {
        audio_on();
        show_toggle(QK_AUDIO_TOGGLE, true);
    }
  return false;
}


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
    case U_AUD_TOG:
      return process_aud_toggle(record);
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
