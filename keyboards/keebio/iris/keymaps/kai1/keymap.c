#include QMK_KEYBOARD_H
#include "print.h"

#define _OS_LAYOUT( \
      idx, nextdflt,  kc_cmd,         kc_foo,         cmd,            foo_T,          redo,           delword,        back,           forward) \
  [idx] = LAYOUT(                                                                                                                                                                                                                     \
      KC_1,           KC_2,           KC_3,           KC_4,           KC_5,           KC_6,                                           KC_7,           KC_8,           KC_9,           KC_0,           KC_MINS,        KC_1,           \
      foo_T(KC_GRV),  KC_Q,           KC_W,           KC_E,           KC_R,           KC_T,                                           KC_Y,           KC_U,           KC_I,           KC_O,           KC_P,           KC_LBRC,        \
      LCTL_T(KC_ESC), KC_A,           KC_S,           KC_D,           KC_F,           KC_G,                                           KC_H,           KC_J,           KC_K,           KC_L,           KC_SCLN,        KC_QUOT,        \
      kc_cmd,         KC_Z,           KC_X,           KC_C,           KC_V,           KC_B,           KC_MPLY,        RGB_TOG,        KC_N,           KC_M,           KC_COMM,        KC_DOT,         KC_SLSH,        KC_ENT,         \
                                                                      MO((idx) + 1),  KC_LSFT,        KC_BSPC,        KC_SPC,         KC_LALT,        MO((idx) + 1)),                                                                 \
  [(idx) + 1] = LAYOUT(                                                                                                                                                                                                               \
      KC_F1,          KC_F2,          KC_F3,          KC_F4,          KC_F5,          KC_F6,                                          KC_F7,          KC_F8,          KC_F9,          KC_F10,         KC_F11,         KC_F12,         \
      kc_foo,         KC_MPRV,        KC_MNXT,        back,           forward,        redo,                                           KC_PGUP,        KC_HOME,        KC_UP,          KC_END,         XXXXXXX,        KC_BSLS,        \
      KC_ESC,         cmd(KC_A),      XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,                                        KC_PGDN,        KC_LEFT,        KC_DOWN,        KC_RGHT,        XXXXXXX,        KC_EQL,         \
      KC_TRNS,        cmd(KC_SLSH),   cmd(KC_B),      cmd(KC_I),      cmd(KC_DOT),    delword,        DF(nextdflt),   DEBUG,          XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        KC_INS,         \
                                                                      KC_TRNS,        KC_TRNS,        KC_DEL,         KC_TAB,         KC_TRNS,        KC_TRNS)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  _OS_LAYOUT(0, 2,         KC_LCTL,      KC_LGUI,      LCTL,         LGUI_T,        LCTL(KC_T),    LCTL(KC_BSPC), LALT(KC_LEFT), LALT(KC_RIGHT)), // Win
  _OS_LAYOUT(2, 0,         KC_LGUI,      KC_LCTL,      LGUI,         LGUI_T,        SGUI(KC_SLSH), LALT(KC_BSPC), LGUI(KC_LEFT), LGUI(KC_RIGHT)), // Mac
};

void _update_rgblight_for_layer(uint32_t state) {
  uint8_t default_layer = biton32(state);
  if (default_layer == 2) { // Mac
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
