// TODO - Copyright

#pragma once


// Keycodes

// Defining an enum here (as recommended in qmk docs) does not work
// This header is included by both c and assembler source
// and the assembly will fail
// Steal some unused keycodes instead

#define U_WIN PB_1
#define U_MAC PB_2
#define U_LNX PB_3
#define U_RGB_TOG PB_4
#define U_RGB_MOD PB_5
#define U_RGB_HUI PB_6
#define U_RGB_SAI PB_7
#define U_RGB_VAI PB_8
#define U_RGB_SPI PB_9


// Layers

#define MIRYOKU_LAYER_MEDIA \
TD(U_TD_BOOT),     TD(U_TD_U_TAP),    TD(U_TD_U_EXTRA),  TD(U_TD_U_BASE),   U_NA,              U_RGB_TOG,         U_RGB_MOD,         U_RGB_HUI,         U_RGB_SAI,         U_RGB_VAI,         \
KC_LGUI,           KC_LALT,           KC_LCTL,           KC_LSFT,           U_NA,              AU_TOGG,           KC_MPRV,           KC_VOLD,           KC_VOLU,           KC_MNXT,           \
U_NA,              KC_ALGR,           TD(U_TD_U_FUN),    TD(U_TD_U_MEDIA),  U_NA,              OU_AUTO,           U_WIN,             U_MAC,             U_LNX,             U_RGB_SPI,         \
U_NP,              U_NP,              U_NA,              U_NA,              U_NA,              KC_MSTP,           KC_MPLY,           KC_MUTE,           U_NP,              U_NP

#define MIRYOKU_LAYER_SYM \
KC_LCBR,           KC_AMPR,           KC_ASTR,           KC_LEAD,           KC_RCBR,           U_NA,              TD(U_TD_U_BASE),   TD(U_TD_U_EXTRA),  TD(U_TD_U_TAP),    TD(U_TD_BOOT),     \
KC_COLN,           KC_DLR,            KC_PERC,           KC_CIRC,           KC_PLUS,           U_NA,              KC_LSFT,           KC_LCTL,           KC_LALT,           KC_LGUI,           \
KC_TILD,           KC_EXLM,           KC_AT,             KC_HASH,           KC_PIPE,           U_NA,              TD(U_TD_U_SYM),    TD(U_TD_U_MOUSE),  KC_ALGR,           U_NA,              \
U_NP,              U_NP,              KC_LPRN,           KC_RPRN,           KC_UNDS,           U_NA,              U_NA,              U_NA,              U_NP,              U_NP


// Sound design

#define AUDIO_INIT_DELAY

#define LAYER_SET_SONG SONG(AG_SWAP_SOUND)
#define MODE_SET_SONG SONG(AG_NORM_SOUND)
#define TOGGLE_ON_SONG SONG(AUDIO_ON_SOUND)
#define TOGGLE_OFF_SONG SONG(AUDIO_OFF_SOUND)
#define DETENT_SONG SONG(Q__NOTE(_A4),)

#define AUDIO_ON_SONG TOGGLE_ON_SONG
#define AUDIO_OFF_SONG TOGGLE_OFF_SONG
#define CG_NORM_SONG SONG(NO_SOUND)
#define CG_SWAP_SONG SONG(NO_SOUND)


// Lighting design

#define RGBLIGHT_DISABLE_KEYCODES
#define RGB_MATRIX_DISABLE_KEYCODES

#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_RAINDROPS
#define RGB_MATRIX_DEFAULT_HUE 0
#define RGB_MATRIX_DEFAULT_SAT 255
#define RGB_MATRIX_DEFAULT_VAL (RGB_MATRIX_MAXIMUM_BRIGHTNESS / 2)
#define RGB_MATRIX_DEFAULT_SPD 80
#define RGB_MATRIX_HUE_STEP 8
#define RGB_MATRIX_SAT_STEP 16
#define RGB_MATRIX_VAL_STEP 16
#define RGB_MATRIX_SPD_STEP 16
