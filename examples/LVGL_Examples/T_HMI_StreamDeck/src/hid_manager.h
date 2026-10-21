/**
 * @file hid_manager.h
 * USB HID output: keyboard combos + consumer (media) control via TinyUSB.
 */
#pragma once

#include <USBHID.h>
#include <USBHIDConsumerControl.h>
#include <USBHIDKeyboard.h>

#include "actions.h"

#if defined(ARDUINO_USB_MODE) && ARDUINO_USB_MODE
#error "TinyUSB HID requires ARDUINO_USB_MODE=0 (see platformio.ini)"
#endif

class HIDManager {
public:
    void begin();
    void send(const ButtonAction &action);

private:
    USBHID _usb; // used to query host readiness
    USBHIDKeyboard _keyboard;
    USBHIDConsumerControl _consumer;
};

extern HIDManager HID;
