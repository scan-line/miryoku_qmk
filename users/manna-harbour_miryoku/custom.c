// TODO - Copyright

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


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


// modifier functions

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

void keyboard_pre_init_user(void) {
  // Replace key overrides with our extended list
  key_overrides = custom_key_overrides;
}
