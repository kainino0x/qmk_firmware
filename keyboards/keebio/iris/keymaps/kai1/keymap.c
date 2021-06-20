#include QMK_KEYBOARD_H
#include "print.h"

// Base layer
#define _OS_LAYOUT(   base,           left,           right,          nextdflt,       cmd,            foo,            foo_grv,        redo,           undo,           cut,            copy,           paste,          delword)        \
  [base] = LAYOUT(                                                                                                                                                                                                                    \
      KC_EXLM,        KC_AT,          KC_HASH,        KC_DLR,         KC_PERC,        KC_CIRC,                                        KC_AMPR,        KC_ASTR,        KC_LPRN,        KC_RPRN,        KC_MINS,        KC_EQL,         \
      foo_grv,        KC_Q,           KC_W,           KC_E,           KC_R,           KC_T,                                           KC_Y,           KC_U,           KC_I,           KC_O,           KC_P,           KC_LBRC,        \
      LCTL_T(KC_ESC), KC_A,           KC_S,           KC_D,           KC_F,           KC_G,                                           KC_H,           KC_J,           KC_K,           KC_L,           KC_SCLN,        KC_QUOT,        \
      cmd,            KC_Z,           KC_X,           KC_C,           KC_V,           KC_B,           KC_MPLY,        RGB_TOG,        KC_N,           KC_M,           KC_COMM,        KC_DOT,         KC_SLSH,        LALT_T(KC_RBRC),\
                                                                      MO(left),       KC_LSFT,        KC_BSPC,        KC_SPC,         KC_ENT,         MO(right)),                                                                     \
  [left] = LAYOUT(                                                                                                                                                                                                                    \
      KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          \
      foo,            KC_NO,          KC_MPRV,        KC_WH_U,        KC_MNXT,        KC_NO,                                          KC_NO,          KC_BTN4,        KC_MS_U,        KC_BTN5,        KC_NO,          KC_NO,          \
      KC_ESC,         redo,           KC_WH_L,        KC_WH_D,        KC_WH_R,        KC_NO,                                          KC_NO,          KC_MS_L,        KC_MS_D,        KC_MS_R,        KC_NO,          KC_NO,          \
      cmd,            undo,           cut,            copy,           paste,          delword,        DF(nextdflt),   DEBUG,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_LALT,        \
                                                                      KC_TRNS,        KC_TRNS,        KC_NO,          KC_TAB,         KC_BTN1,        KC_BTN2),                                                                       \
  [right] = LAYOUT(                                                                                                                                                                                                                   \
      KC_F1,          KC_F2,          KC_F3,          KC_F4,          KC_F5,          KC_F6,                                          KC_F7,          KC_F8,          KC_F9,          KC_F10,         KC_F11,         KC_F12,         \
      foo,            KC_NO,          KC_P7,          KC_P8,          KC_P9,          KC_NO,                                          KC_PGUP,        KC_HOME,        KC_UP,          KC_END,         KC_NO,          KC_BSLS,        \
      KC_ESC,         KC_NO,          KC_P4,          KC_P5,          KC_P6,          KC_NO,                                          KC_PGDN,        KC_LEFT,        KC_DOWN,        KC_RGHT,        KC_NO,          KC_NO,          \
      cmd,            KC_P0,          KC_P1,          KC_P2,          KC_P3,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_LALT,        \
                                                                      KC_NO,          KC_TRNS,        KC_DEL,         KC_NO,          KC_INS,         KC_TRNS)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  _OS_LAYOUT(         0,              1,              2,              3,              KC_LCTL,        KC_LGUI,        LGUI_T(KC_GRV), LCTL(KC_T),     LCTL(KC_SLSH),  LCTL(KC_B),     LCTL(KC_I),     LCTL(KC_DOT),   LCTL(KC_BSPC)), // Win
  _OS_LAYOUT(         3,              4,              5,              0,              KC_LGUI,        KC_LCTL,        LCTL_T(KC_GRV), SGUI(KC_SLSH),  LGUI(KC_SLSH),  LGUI(KC_B),     LGUI(KC_I),     LGUI(KC_DOT),   LALT(KC_BSPC)), // Mac
};

void _update_rgblight_for_layer(uint32_t state) {
  uint8_t default_layer = biton32(state);
  if (default_layer & 1) { // Mac
    rgblight_sethsv_noeeprom(132, 0, rgblight_get_val());
  } else { // Win
    rgblight_sethsv_noeeprom(132, 255, rgblight_get_val());
  }
}
void _update_rgblight_for_debug(void) {
  rgblight_mode_noeeprom(debug_enable ? (RGBLIGHT_MODE_BREATHING + 2) : RGBLIGHT_MODE_STATIC_LIGHT);
}

// EVENT HANDLERS

void eeconfig_init_user(void) {
}

void keyboard_post_init_user(void) {
  debug_enable = true;
  _update_rgblight_for_layer(default_layer_state);
  _update_rgblight_for_debug();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if ((keycode & 0xFF00) == QK_DEF_LAYER) {
    uint8_t layer = keycode; // lower 8 bits are the layer number
    dprintf("set_single_persistent_default_layer %u\n", layer);
    set_single_persistent_default_layer(layer);
    return true;
  }
  if (keycode == DEBUG) {
    _update_rgblight_for_debug();
    return true;
  }
  return true;
}

uint32_t default_layer_state_set_user(uint32_t state) {
  _update_rgblight_for_layer(state);
  return state;
}

bool encoder_update_user(uint8_t index, bool clockwise) {
  if (index == 0) { // Left encoder
    if (clockwise) {
      tap_code(KC_VOLU);
    } else {
      tap_code(KC_VOLD);
    }
  } else if (index == 1) { // Right encoder
    if (clockwise) {
      rgblight_increase_val();
    } else {
      rgblight_decrease_val();
    }
  }
  return false;
}
