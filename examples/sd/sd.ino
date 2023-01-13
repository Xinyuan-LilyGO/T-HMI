#include "pins.h"
#include <Arduino.h>
#include <SD_MMC.h>
#include <FS.h>
#include <SPI.h>
#include <SD.h>

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(PWR_EN_PIN, OUTPUT);
    digitalWrite(PWR_EN_PIN, HIGH);
    SD_MMC.setPins(SD_SCLK_PIN, SD_MOSI_PIN, SD_MISO_PIN);
    bool rlst = SD_MMC.begin("/sdcard", true);
    if (!rlst) {
        Serial.println("SD init failed");
        Serial.println("➸ No detected SdCard");
    } else {
        Serial.println("SD init success");
        Serial.printf("➸ Detected SdCard insert: %.2f GB\r\n", SD_MMC.cardSize() / 1024.0 / 1024.0 / 1024.0);
    }
}

void loop() {
    delay(10);
}
