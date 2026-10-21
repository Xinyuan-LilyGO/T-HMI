/**
 * @file actions.h
 * HID action model shared by the configuration table and the HID manager.
 *
 * This header is intentionally free of LVGL and Arduino dependencies so the
 * action table can be validated on any host.
 */
#pragma once

#include <cstdint>

#include <class/hid/hid.h>

// Keyboard modifier bitmask. This is NOT a USB HID modifier byte; HIDManager
// translates each bit to the matching HID_KEY_* left-hand modifier.
enum Mod : uint8_t {
    MOD_NONE = 0x00,
    MOD_CTRL = 0x01,
    MOD_SHIFT = 0x02,
    MOD_ALT = 0x04,
    MOD_GUI = 0x08, // Windows / Command key
};

constexpr uint8_t MOD_ALL =
    static_cast<uint8_t>(MOD_CTRL | MOD_SHIFT | MOD_ALT | MOD_GUI);

// A key press/combo sent over the USB HID keyboard interface.
struct KeyAction {
    uint8_t mods;    // MOD_* bitmask
    uint8_t keycode; // HID_KEY_* usage ID, 0 for modifier-only actions
};

// Consumer/media usage codes (USB HID Consumer Control page 0x0C).
enum ConsumerCode : uint16_t {
    CONSUMER_NONE = 0,
    CONSUMER_PLAY_PAUSE = 0xCD,
    CONSUMER_SCAN_NEXT = 0xB5,
    CONSUMER_SCAN_PREV = 0xB6,
    CONSUMER_VOL_UP = 0xE9,
    CONSUMER_VOL_DOWN = 0xEA,
    CONSUMER_MUTE = 0xE2,
};

// One button action: a key combo, a consumer code, or both.
// The consumer code is stored as uint16_t (its HID usage ID) so that constant
// evaluation works on older GCC toolchains used by the ESP32 Arduino core.
struct ButtonAction {
    KeyAction key;
    uint16_t consumer;
};

constexpr ButtonAction Keys(uint8_t mods, uint8_t keycode) {
    return ButtonAction{{mods, keycode}, CONSUMER_NONE};
}

constexpr ButtonAction Media(ConsumerCode code) {
    return ButtonAction{{MOD_NONE, 0}, code};
}

constexpr bool is_valid_mod_mask(uint8_t mods) {
    return (mods & static_cast<uint8_t>(~MOD_ALL)) == 0;
}

constexpr bool action_has_key(const ButtonAction &action) {
    return action.key.keycode != 0 || action.key.mods != MOD_NONE;
}

constexpr bool action_is_valid(const ButtonAction &action) {
    if (!is_valid_mod_mask(action.key.mods)) return false;
    return action_has_key(action) || action.consumer != CONSUMER_NONE;
}
