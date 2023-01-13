#include "pins.h"

#include <Arduino.h>
#include <esp_adc_cal.h>


void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(PWR_EN_PIN, OUTPUT);
    digitalWrite(PWR_EN_PIN, HIGH);
    delay(100);
}


void loop() {
    uint32_t voltage = readAdcVoltage(BAT_ADC_PIN) * 2;
    Serial.print("Battery voltage: ");
    Serial.println((float)voltage/1000);
    delay(3000);
}


uint32_t readAdcVoltage(int pin) {
    esp_adc_cal_characteristics_t adc_chars;

    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    return esp_adc_cal_raw_to_voltage(analogRead(pin), &adc_chars);
}