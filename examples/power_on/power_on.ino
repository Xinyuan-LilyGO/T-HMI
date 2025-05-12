#include "pins.h"

#include <OneButton.h>
#include <Arduino.h>

OneButton buttonOK(21, false, false);

/*
* Power on/off method:
* 1. Use the onboard ON/OFF switch to switch the machine on and off
* 2. Connect an external switch, see here https://github.com/Xinyuan-LilyGO/T-HMI/issues/34
* 3. Use a magnet to approach the reed switch to switch
* */
void setup() {
    pinMode(PWR_ON_PIN, OUTPUT);
    digitalWrite(PWR_ON_PIN, HIGH);

    Serial.begin(115200);
    delay(1000);

    pinMode(PWR_EN_PIN, OUTPUT);
    digitalWrite(PWR_EN_PIN, HIGH);
    pinMode(BK_LIGHT_PIN, OUTPUT);
    digitalWrite(BK_LIGHT_PIN, HIGH);
    buttonOK.attachClick(power_off);
    buttonOK.attachLongPressStop(power_off);
    // pinMode(21, INPUT)
    Serial.println("setup");
}


void power_off() {
    digitalWrite(PWR_ON_PIN, LOW);
    Serial.println("power_off");
}


void loop() {
    buttonOK.tick();
    delay(10);
}
