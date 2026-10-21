#include "backlight.h"

#include <Arduino.h>

namespace backlight {

void init(uint8_t pin) {
    ledcSetup(LEDC_CHANNEL, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);
    ledcAttachPin(pin, LEDC_CHANNEL);
    ledcWrite(LEDC_CHANNEL, 0);
}

void set_dimmed(bool dimmed) {
    ledcWrite(LEDC_CHANNEL, dimmed ? DUTY_DIM : DUTY_FULL);
}

} // namespace backlight
