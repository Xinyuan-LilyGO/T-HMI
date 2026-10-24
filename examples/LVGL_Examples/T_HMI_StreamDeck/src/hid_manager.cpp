/**
 * @file hid_manager.cpp
 * USB HID output implementation.
 */
#include "hid_manager.h"

#include <Arduino.h>

namespace {

constexpr uint32_t CONSUMER_HOLD_MS = 5;
constexpr uint32_t COMBO_HOLD_MS = 8;
constexpr uint32_t NOT_READY_LOG_INTERVAL_MS = 5000;

struct ModifierMap {
    uint8_t mask;
    uint8_t keycode;
};

// pressRaw() expects TinyUSB HID_KEY_* codes (0xE0-0xE7 for modifiers), not
// the internal MOD_* bitmask.
constexpr ModifierMap MODIFIER_MAP[] = {
    {MOD_CTRL, HID_KEY_CONTROL_LEFT},
    {MOD_SHIFT, HID_KEY_SHIFT_LEFT},
    {MOD_ALT, HID_KEY_ALT_LEFT},
    {MOD_GUI, HID_KEY_GUI_LEFT},
};
constexpr size_t MODIFIER_MAP_SIZE = sizeof(MODIFIER_MAP) / sizeof(MODIFIER_MAP[0]);

uint32_t last_not_ready_log_ms = 0;

void log_not_ready() {
    const uint32_t now = millis();
    if (now - last_not_ready_log_ms < NOT_READY_LOG_INTERVAL_MS) return;
    last_not_ready_log_ms = now;
    Serial.println("[HID] USB host not ready; press ignored");
}

} // namespace

HIDManager HID;

void HIDManager::begin() {
    // Consumer first so the composite descriptor includes both interfaces.
    _consumer.begin();
    _keyboard.begin();
    Serial.println("[HID] USB HID keyboard + consumer control started");
}

void HIDManager::send(const ButtonAction &action) {
    if (!_usb.ready()) {
        log_not_ready();
        return;
    }

    if (action.consumer != CONSUMER_NONE) {
        _consumer.press(action.consumer);
        delay(CONSUMER_HOLD_MS);
        _consumer.release();
    }

    if (!action_has_key(action)) return;

    for (const ModifierMap &modifier : MODIFIER_MAP) {
        if (action.key.mods & modifier.mask) _keyboard.pressRaw(modifier.keycode);
    }
    if (action.key.keycode) _keyboard.pressRaw(action.key.keycode);

    delay(COMBO_HOLD_MS); // let the host register the combo

    if (action.key.keycode) _keyboard.releaseRaw(action.key.keycode);
    for (size_t i = MODIFIER_MAP_SIZE; i-- > 0;) {
        if (action.key.mods & MODIFIER_MAP[i].mask) _keyboard.releaseRaw(MODIFIER_MAP[i].keycode);
    }
}
