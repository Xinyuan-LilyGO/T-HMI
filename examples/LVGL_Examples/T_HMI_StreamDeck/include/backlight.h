#pragma once

#include <cstdint>

namespace backlight {

constexpr uint8_t LEDC_CHANNEL = 0;
constexpr uint32_t PWM_FREQUENCY_HZ = 5000;
constexpr uint8_t PWM_RESOLUTION_BITS = 8;
constexpr uint32_t DUTY_FULL = (1u << PWM_RESOLUTION_BITS) - 1u;
constexpr uint32_t DUTY_DIM = 40;

void init(uint8_t pin);
void set_dimmed(bool dimmed);

} // namespace backlight
