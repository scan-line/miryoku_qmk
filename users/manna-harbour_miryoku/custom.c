// TODO - Copyright

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


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

os_mode_t os_mode_get(void)
{
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


// Layer indication

float layer_set_song[][2] = LAYER_SET_SONG;

layer_state_t default_layer_state_set_user(layer_state_t state)
{
  PLAY_SONG(layer_set_song);
  return state;
}


// Custom key processing

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
    default:
      return true;
  }
}


// Initialization

void eeconfig_init_user(void) {
  user_config.raw = 0;
  // Windows mode by default
  // keymap_config.swap_lctl_lgui defaults to false
  user_config.os_mode_linux = false;
  
  eeconfig_update_user(user_config.raw);
}

void keyboard_post_init_user(void) {
  // Replace key overrides with our extended list
  key_overrides = custom_key_overrides;
  // Restore user state
  user_config.raw = eeconfig_read_user();
  os_mode = os_mode_get();
}
