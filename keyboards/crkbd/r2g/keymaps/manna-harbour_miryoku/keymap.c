// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


// Qmk definitions

// Oneshot no-ops
#ifdef NO_ACTION_ONESHOT
#  define get_oneshot_mods() 0
#endif


// Luna, keyboard pet
// https://github.com/HellSingCoder/qmk_firmware/blob/master/keyboards/sofle/keymaps/helltm/keymap.c

// Settings
#define MIN_WALK_SPEED 10  // wpm
#define MIN_RUN_SPEED 40
// Advanced settings
#define ANIM_FRAME_DURATION 200  // ms
#define ANIM_SIZE 96

// Status variables
bool isJumping  = false;
bool showJump = false;

static void render_luna(int x, int y) {
  // Frames 32x22px
  // Sit
  static const char PROGMEM sit[2][ANIM_SIZE] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x1c,
      0x02, 0x05, 0x02, 0x24, 0x04, 0x04, 0x02, 0xa9, 0x1e, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x10, 0x08, 0x68, 0x10, 0x08, 0x04, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x06, 0x82, 0x7c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x04, 0x0c, 0x10, 0x10, 0x20, 0x20, 0x20, 0x28,
      0x3e, 0x1c, 0x20, 0x20, 0x3e, 0x0f, 0x11, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x1c,
      0x02, 0x05, 0x02, 0x24, 0x04, 0x04, 0x02, 0xa9, 0x1e, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x90, 0x08, 0x18, 0x60, 0x10, 0x08, 0x04, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0e, 0x82, 0x7c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x04, 0x0c, 0x10, 0x10, 0x20, 0x20, 0x20, 0x28,
      0x3e, 0x1c, 0x20, 0x20, 0x3e, 0x0f, 0x11, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
  };
  // Walk
  static const char PROGMEM walk[2][ANIM_SIZE] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x40, 0x20, 0x10, 0x90, 0x90, 0x90, 0xa0, 0xc0, 0x80, 0x80,
      0x80, 0x70, 0x08, 0x14, 0x08, 0x90, 0x10, 0x10, 0x08, 0xa4, 0x78, 0x80, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x08, 0xfc, 0x01, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x18, 0xea, 0x10, 0x0f, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x1c, 0x20, 0x20, 0x3c, 0x0f, 0x11, 0x1f, 0x03,
      0x06, 0x18, 0x20, 0x20, 0x3c, 0x0c, 0x12, 0x1e, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x40, 0x20, 0x20, 0x20, 0x40, 0x80, 0x00, 0x00, 0x00,
      0x00, 0xe0, 0x10, 0x28, 0x10, 0x20, 0x20, 0x20, 0x10, 0x48, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x20, 0xf8, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
      0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10, 0x30, 0xd5, 0x20, 0x1f, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x20, 0x30, 0x0c, 0x02, 0x05, 0x09, 0x12, 0x1e,
      0x02, 0x1c, 0x14, 0x08, 0x10, 0x20, 0x2c, 0x32, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
  };
  // Run
  static const char PROGMEM run[2][ANIM_SIZE] = {
    { 0x00, 0x00, 0x00, 0x00, 0xe0, 0x10, 0x08, 0x08, 0xc8, 0xb0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x40, 0x40, 0x3c, 0x14, 0x04, 0x08, 0x90, 0x18, 0x04, 0x08, 0xb0, 0x40, 0x80, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0xc4, 0xa4, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc8, 0x58, 0x28, 0x2a, 0x10, 0x0f, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x09, 0x04, 0x04, 0x04, 0x04, 0x02, 0x03, 0x02, 0x01, 0x01,
      0x02, 0x02, 0x04, 0x08, 0x10, 0x26, 0x2b, 0x32, 0x04, 0x05, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, },
    { 0x00, 0x00, 0x00, 0xe0, 0x10, 0x10, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
      0x80, 0x80, 0x78, 0x28, 0x08, 0x10, 0x20, 0x30, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x03, 0x04, 0x08, 0x10, 0x11, 0xf9, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0xb0, 0x50, 0x55, 0x20, 0x1f, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x0c, 0x10, 0x20, 0x28, 0x37,
      0x02, 0x1e, 0x20, 0x20, 0x18, 0x0c, 0x14, 0x1e, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
  }; 
  // Bark
  static const char PROGMEM bark[2][ANIM_SIZE] = {
    { 0x00, 0xc0, 0x20, 0x10, 0xd0, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x40,
      0x3c, 0x14, 0x04, 0x08, 0x90, 0x18, 0x04, 0x08, 0xb0, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x03, 0x04, 0x08, 0x10, 0x11, 0xf9, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x80, 0xc8, 0x48, 0x28, 0x2a, 0x10, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x0c, 0x10, 0x20, 0x28, 0x37, 0x02, 0x02,
      0x04, 0x08, 0x10, 0x26, 0x2b, 0x32, 0x04, 0x05, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
    { 0x00, 0xe0, 0x10, 0x10, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x40,
      0x40, 0x2c, 0x14, 0x04, 0x08, 0x90, 0x18, 0x04, 0x08, 0xb0, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x03, 0x04, 0x08, 0x10, 0x11, 0xf9, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x48, 0x28, 0x2a, 0x10, 0x0f, 0x20, 0x4a, 0x09, 0x10,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x0c, 0x10, 0x20, 0x28, 0x37, 0x02, 0x02,
      0x04, 0x08, 0x10, 0x26, 0x2b, 0x32, 0x04, 0x05, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
  };
  // Sneak
  static const char PROGMEM sneak[2][ANIM_SIZE] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x40, 0x40, 0x40, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0xc0, 0x40, 0x40, 0x80, 0x00, 0x80, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x1e, 0x21, 0xf0, 0x04, 0x02, 0x02, 0x02, 0x02, 0x03, 0x02, 0x02, 0x04,
      0x04, 0x04, 0x03, 0x01, 0x00, 0x00, 0x09, 0x01, 0x80, 0x80, 0xab, 0x04, 0xf8, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x1c, 0x20, 0x20, 0x3c, 0x0f, 0x11, 0x1f, 0x02, 0x06,
      0x18, 0x20, 0x20, 0x38, 0x08, 0x10, 0x18, 0x04, 0x04, 0x02, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x40, 0x40, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0xe0, 0xa0, 0x20, 0x40, 0x80, 0xc0, 0x20, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x3e, 0x41, 0xf0, 0x04, 0x02, 0x02, 0x02, 0x03, 0x02, 0x02, 0x02, 0x04,
      0x04, 0x02, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x40, 0x40, 0x55, 0x82, 0x7c, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x20, 0x30, 0x0c, 0x02, 0x05, 0x09, 0x12, 0x1e, 0x04,
      0x18, 0x10, 0x08, 0x10, 0x20, 0x28, 0x34, 0x06, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, },
  }; 

  // Animation
  void animate_luna(void) {
    // Clear
    if (isJumping || showJump) {
      oled_set_cursor(x, y + 2);
      oled_write("     ", false);
      oled_set_cursor(x, y - 1);
      showJump = false;
    } else {
      oled_set_cursor(x, y - 1);
      oled_write("     ", false);
      oled_set_cursor(x, y);
    }

    // Switch frame
    static uint8_t current_frame = 0;
    current_frame = (current_frame + 1) % 2;

    // Current status
    const int current_wpm = get_current_wpm();
    const led_t led_state = host_keyboard_led_state();
    const bool isSneaking = (get_mods() | get_weak_mods() | get_oneshot_mods()) & (MOD_MASK_CTRL | MOD_MASK_GUI);

    if (led_state.caps_lock || is_caps_word_on()) {
      oled_write_raw_P(bark[current_frame], ANIM_SIZE);
    } else if (isSneaking) {
      oled_write_raw_P(sneak[current_frame], ANIM_SIZE);
    } else if (current_wpm <= MIN_WALK_SPEED) {
      oled_write_raw_P(sit[current_frame], ANIM_SIZE);
    } else if (current_wpm <= MIN_RUN_SPEED) {
      oled_write_raw_P(walk[current_frame], ANIM_SIZE);
    } else {
      oled_write_raw_P(run[current_frame], ANIM_SIZE);
    }
  }

#if OLED_TIMEOUT > 0
  // The animation prevents the normal timeout from occuring
  if (last_input_activity_elapsed() > OLED_TIMEOUT && last_led_activity_elapsed() > OLED_TIMEOUT) {
    oled_off();
    return;
  } else {
    oled_on();
  }
#endif

  // Animation timer
  static uint32_t anim_timer = 0;
  if (timer_elapsed32(anim_timer) > ANIM_FRAME_DURATION) {
    anim_timer = timer_read32();
    animate_luna();
  }
}

void process_record_luna(uint16_t keycode, keyrecord_t *record) {
  // Space is also on LT(U_NAV,KC_SPC).
  if (IS_QK_LAYER_TAP(keycode) && record->tap.count > 0) {
    keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
  }

  switch (keycode) {
    case KC_SPC:
      if (record->event.pressed) {
        isJumping  = true;
        showJump = true;
      } else {
        isJumping  = false;
      }
      break;
    default:
      break;
  }
}


// Feedback

const char empty_message[] PROGMEM = "   ";
const char* base_message = empty_message;
const char* message = empty_message;

#define MESSAGE_DURATION 2000  // ms
uint32_t message_timer = 0;

void set_message(const char* str) {
  base_message = str;
  message = str;
}

void flash_message(const char* str) {
  message_timer = timer_read32();
  message = str;
}

void render_message(int x, int y) {
  if (message != base_message && timer_elapsed32(message_timer) > MESSAGE_DURATION)
    message = base_message;
  
  oled_set_cursor(x, y);
  oled_write_P(message, false);
}

void show_mode_custom(uint16_t keycode) {
  switch (keycode) {
    case U_WIN:
      flash_message(PSTR("Win"));
      break;
    case U_MAC:
      flash_message(PSTR("Mac"));
      break;
    case U_LNX:
      flash_message(PSTR("Lnx"));
      break;
    default:
      break;
  }
}

void show_default_layer_custom(uint8_t layer) {
  set_message(empty_message);
  switch (layer) {
    case U_BASE:
      flash_message(PSTR("Cmk"));
      break;
    case U_EXTRA:
      flash_message(PSTR("Qty"));
      break;
    case U_TAP:
      flash_message(PSTR("Tap"));
      break;
    case U_BUTTON:
      set_message(PSTR("Btn"));
      break;
    case U_NAV:
      set_message(PSTR("Nav"));
      break;
    case U_MOUSE:
      set_message(PSTR("Mse"));
      break;
    case U_MEDIA:
      set_message(PSTR("Med"));
      break;
    case U_NUM:
      set_message(PSTR("Num"));
      break;
    case U_SYM:
      set_message(PSTR("Sym"));
      break;
    case U_FUN:
      set_message(PSTR("Fun"));
      break;
    default:
      break;
  }
}

void show_toggle_custom(uint16_t keycode, bool value) {
  if (value)
    flash_message(PSTR("[x]"));
  else
    flash_message(PSTR("[ ]"));
}

void show_value_custom(uint16_t keycode, uint16_t value, bool detent) {
  if (detent)
    flash_message(PSTR("-=-"));
  else
    set_message(base_message);
}


// Logos

static const char PROGMEM miryoku_logo[] = {
  0x00, 0xfe, 0xfe, 0x06, 0x06, 0xe6, 0xe6, 0x66, 0x66, 0xe6, 0xe6, 0x66, 0x66, 0xe6, 0xe6, 0x06, 
  0x06, 0xe6, 0xe6, 0x86, 0x86, 0xe6, 0xe6, 0x86, 0x86, 0xe6, 0xe6, 0x06, 0x06, 0xfe, 0xfe, 0x00, 
  0x00, 0xff, 0xff, 0x00, 0x00, 0x7f, 0x7f, 0x66, 0x66, 0xff, 0xff, 0x66, 0x66, 0x7f, 0x7f, 0x00, 
  0x00, 0x79, 0x79, 0x61, 0x61, 0xff, 0xff, 0x61, 0x61, 0x79, 0x79, 0x00, 0x00, 0xff, 0xff, 0x00, 
  0x00, 0xff, 0xff, 0x00, 0x00, 0xfe, 0xfe, 0x06, 0x06, 0xff, 0xff, 0x66, 0x66, 0x7e, 0x7e, 0x00, 
  0x00, 0xfe, 0xfe, 0x06, 0x06, 0xff, 0xff, 0x06, 0x06, 0xfe, 0xfe, 0x00, 0x00, 0xff, 0xff, 0x00, 
  0x00, 0x7f, 0x7f, 0x60, 0x60, 0x67, 0x67, 0x60, 0x60, 0x67, 0x67, 0x66, 0x66, 0x66, 0x66, 0x66, 
  0x66, 0x67, 0x67, 0x60, 0x60, 0x67, 0x67, 0x60, 0x60, 0x67, 0x67, 0x60, 0x60, 0x7f, 0x7f, 0x00,
};

void render_miryoku_logo(int x, int y) {
  oled_set_cursor(x, y);
  oled_write_raw_P(miryoku_logo, sizeof(miryoku_logo));
}


// Oled

void oled_task_left(void) {
  render_message(1, 1);
  render_luna(0, 6);
}

void oled_task_right(void) {
  render_miryoku_logo(0, 5);
}

bool oled_task_kb(void) {
  if (!oled_task_user()) {
    return false;
  }
  if (is_keyboard_left()) {
    oled_task_left();
  } else {
    oled_set_brightness(16);
    oled_task_right();
  }
  return false;
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
  process_record_luna(keycode, record);
  return process_record_user(keycode, record);
}

oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
  return OLED_ROTATION_270;
}
