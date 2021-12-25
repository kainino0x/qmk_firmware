#include QMK_KEYBOARD_H
#include "print.h"
#include "dvorak.h"

#define _OS_LAYOUT( \
      i, nextdflt,    kc_cmd,         cmd,            kc_other,       m_swi,          redo,           word,           nav_bck,        nav_fwd,        line_home,      line_end) \
  [i] = LAYOUT( /* base layer */                                                                                                                                                                                                      \
      KC_GRV,         KC_1,           KC_2,           KC_3,           KC_4,           KC_5,                                           KC_6,           KC_7,           KC_8,           KC_9,           KC_0,           DV_EQL,         \
      KC_TAB,         DV_QUOT,        DV_COMM,        DV_DOT,         DV_P,           DV_Y,                                           DV_F,           DV_G,           DV_C,           DV_R,           DV_L,           DV_SLSH,        \
      LCTL_T(KC_ESC), DV_A,           DV_O,           DV_E,           DV_U,           DV_I,                                           DV_D,           DV_H,           DV_T,           DV_N,           DV_S,           DV_MINS,        \
      LM(i+2,m_swi),  DV_SCLN,        DV_Q,           DV_J,           DV_K,           DV_X,           KC_MPLY,        KC_RALT,        DV_B,           DV_M,           DV_W,           DV_V,           DV_Z,           KC_ENT,         \
                                                                      kc_cmd,         KC_LSFT,        MO(i+1),        MO(i+1),        KC_SPC,         KC_BSPC),                                                                       \
  [i+1] = LAYOUT( /* "lower" layer */                                                                                                                                                                                                 \
      LCTL(KC_PGUP),  KC_F1,          KC_F2,          KC_F3,          KC_F4,          KC_F5,                                          KC_F6,          KC_F7,          KC_F8,          DV_LBRC,        DV_RBRC,        DEBUG,          \
      LCTL(KC_PGDN),  XXXXXXX,        line_home,      KC_PGUP,        line_end,       redo,                                           nav_fwd,        KC_HOME,        KC_UP,          KC_END,         KC_MNXT,        DV_BSLS,        \
      KC_ESC,         cmd(DV_A),      word(KC_LEFT),  KC_PGDN,        word(KC_RGHT),  KC_INS,                                         XXXXXXX,        KC_LEFT,        KC_DOWN,        KC_RGHT,        KC_MPRV,        XXXXXXX,        \
      kc_other,       cmd(DV_Z),      cmd(DV_X),      cmd(DV_C),      cmd(DV_V),      KC_DEL,         DF(nextdflt),   RGB_TOG,        nav_bck,        KC_F9,          KC_F10,         KC_F11,         KC_F12,         XXXXXXX,        \
                                                                      _______,        _______,        TG(i+1),        TG(i+1),        _______,        word(KC_BSPC)),                                                                 \
  [i+2] = LAYOUT( /* layer just for window switching */                                                                                                                                                                               \
      LSFT(KC_TAB),   XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,                                        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        \
      KC_TAB,         XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,                                        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        \
      XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,                                        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        \
      _______,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        \
                                                                      XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  _OS_LAYOUT(0, 4,    KC_LCTL,        LCTL,           KC_LGUI,        MOD_LALT,       LCTL(DV_Y),     LCTL,           LALT(KC_LEFT),  LALT(KC_RIGHT), KC_HOME,        KC_END), // Win
  _OS_LAYOUT(4, 0,    KC_LGUI,        LGUI,           KC_LCTL,        MOD_LGUI,       SGUI(DV_Z),     LALT,           LGUI(KC_LEFT),  LGUI(KC_RIGHT), LCTL(KC_A),     LCTL(KC_E)), // Mac
};

uint8_t _last_rgblight_sat = 0;
uint8_t _last_rgblight_val = 0;

void _update_rgblight_for_layer(uint32_t state) {
  switch (get_highest_layer(state) % 4) {
    case 0: // Base layer (persistent layer number isn't visible here)
      rgblight_sethsv_noeeprom(132, _last_rgblight_sat, _last_rgblight_val);
      break;
    case 1: // Lower
      rgblight_sethsv_noeeprom(200, 255, _last_rgblight_val);
      break;
  }
}
void _update_rgblight_for_debug(void) {
  static bool _saved_rgblight_enable = false;
  if (debug_enable) {
    _saved_rgblight_enable = rgblight_is_enabled();
    rgblight_enable();
    rgblight_mode_noeeprom(RGBLIGHT_MODE_BREATHING + 2);
  } else {
    if (!_saved_rgblight_enable) rgblight_disable();
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
  }
}

// EVENT HANDLERS

void eeconfig_init_user(void) {
}

void keyboard_post_init_user(void) {
  _last_rgblight_sat = rgblight_get_sat();
  _last_rgblight_val = rgblight_get_val();
  _update_rgblight_for_layer(default_layer_state);
  _update_rgblight_for_debug();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if ((keycode & 0xFF00) == QK_DEF_LAYER) {
    uint8_t layer = keycode; // lower 8 bits are the layer number
    dprintf("set_single_persistent_default_layer %u\n", layer);
    set_single_persistent_default_layer(layer);
    _last_rgblight_sat = layer ? 0 : 255; // Win=blue, Mac=white
    return true;
  }
  if (keycode == DEBUG) {
    _update_rgblight_for_debug();
    return true;
  }
  return true;
}

uint32_t layer_state_set_user(uint32_t state) {
  _update_rgblight_for_layer(state);
  return state;
}

bool encoder_update_user(uint8_t index, bool clockwise) {
  bool is_lower = IS_LAYER_ON(1) || IS_LAYER_ON(3);
  if (index == 0) { // Left encoder
    if (clockwise) {
      tap_code(KC_VOLU);
    } else {
      tap_code(KC_VOLD);
    }
  } else if (index == 1) { // Right encoder
    if (clockwise) {
      if (is_lower) {
        rgblight_increase_val();
        _last_rgblight_val = rgblight_get_val();
      } else {
        tap_code16(LCTL(KC_PGDN));
      }
    } else {
      if (is_lower) {
        rgblight_decrease_val();
        if (rgblight_get_val() < 32) {
          rgblight_sethsv(rgblight_get_hue(), rgblight_get_sat(), 32);
        }
        _last_rgblight_val = rgblight_get_val();
      } else {
        tap_code16(LCTL(KC_PGUP));
      }
    }
  }
  return false;
}
