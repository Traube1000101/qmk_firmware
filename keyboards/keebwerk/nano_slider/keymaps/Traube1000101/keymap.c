/* Copyright 2020 Duckle
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include "analog.h"
#include "qmk_midi.h"

uint8_t midi2vol = 0x3E;

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _F_KEYS,
    _VOLUME,
    _LIGHTS,
    _FN,
};

// Defines the keycodes used by our macros in process_record_user
enum custom_keycodes {
    DEFAULT = SAFE_RANGE,
    SPOTIFY,
    DISCORD,
    BOOTLOADER_1,
    BOOTLOADER_2,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_F_KEYS] = LAYOUT(
        MO(_FN),
        KC_F13, KC_F14, KC_F15,
        KC_F16, KC_F17, KC_F18, KC_F19
    ),
    [_VOLUME] = LAYOUT(
        MO(_FN),
        DEFAULT, SPOTIFY, DISCORD,
        KC_NO, KC_NO, KC_NO, KC_NO
    ),
    [_LIGHTS] = LAYOUT(
        MO(_FN),
        RGB_TOG, RGB_MOD, RGB_VAI,
        RGB_SAI, BL_BRTG, BL_STEP, KC_LSFT
    ),
    [_FN] = LAYOUT(
        MO(_FN),
        TO(_F_KEYS), TO(_VOLUME), TO(_LIGHTS),
        DEFAULT, KC_NO, BOOTLOADER_1, BOOTLOADER_2
    )
};
// clang-format on

static bool bootloader_1_held = false;
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DEFAULT:
            if (record->event.pressed) {
                midi2vol = 0x3E;
            }
            break;
        case SPOTIFY:
            if (record->event.pressed) {
                midi2vol = 0x3F;
            }
            break;
        case DISCORD:
            if (record->event.pressed) {
                midi2vol = 0x40;
            }
            break;
        case BOOTLOADER_1:
            bootloader_1_held = record->event.pressed;
            break;
        case BOOTLOADER_2:
            if (record->event.pressed && bootloader_1_held) {
                reset_keyboard();
            }
            break;
    }
    return true;
}

uint8_t divisor = 0;

void slider(void) {
    if (divisor++) { // only run the slider function 1/256 times it's called
        return;
    }

    midi_send_cc(&midi_device, 2, midi2vol, 0x7F - (analogReadPin(SLIDER_PIN) >> 3));
}

void matrix_scan_user(void) {
    slider();
}
