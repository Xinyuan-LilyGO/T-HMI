#include "pins.h"
#include <SPI.h>
#include <Arduino.h>
#include <xpt2046.h>

#if __has_include("data.h")
#include "data.h"
#define USE_CALIBRATION_DATA 1
#endif

#if __has_include(<TFT_eSPI.h>)
#include <TFT_eSPI.h>
#define USE_TFT_eSPI 1
#endif

XPT2046 touch = XPT2046(SPI, TOUCHSCREEN_CS_PIN, TOUCHSCREEN_IRQ_PIN);

#if USE_CALIBRATION_DATA
touch_calibration_t calibration_data[4];
#endif

#if USE_TFT_eSPI
TFT_eSPI tft = TFT_eSPI();
#endif

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(PWR_EN_PIN, OUTPUT);
    digitalWrite(PWR_EN_PIN, HIGH);

#if USE_CALIBRATION_DATA
    data_init();
    data_read(calibration_data);
#endif

    SPI.begin(TOUCHSCREEN_SCLK_PIN, TOUCHSCREEN_MISO_PIN, TOUCHSCREEN_MOSI_PIN);
    touch.begin(240, 320);
#if USE_CALIBRATION_DATA
    touch.setCal(calibration_data[0].rawX, calibration_data[2].rawX, calibration_data[0].rawY, calibration_data[2].rawY, 240, 320); // Raw xmin, xmax, ymin, ymax, width, height
#else
    touch.setCal(1788, 285, 1877, 311, 240, 320); // Raw xmin, xmax, ymin, ymax, width, height
    Serial.println("Use default calibration data");
#endif
    touch.setRotation(0);

#if USE_TFT_eSPI
    tft.begin();
    tft.setRotation(0);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("touch test", 40, 100, 2);
#endif
}

String s;

void loop() {
    if (touch.pressed()) {
        s = "RAW X: ";
        s += touch.RawX();
        s += " RAW Y: ";
        s += touch.RawY();
        Serial.println(s);
#if USE_TFT_eSPI
        tft.fillRect(40, 100, 200, 40, TFT_BLACK);
        tft.drawString(s, 40, 100, 2);
#endif
        s = "X: ";
        s += touch.X();
        s += " Y: ";
        s += touch.Y();
        Serial.println(s);
#if USE_TFT_eSPI
        tft.drawString(s, 40, 120, 2);
#endif
        // Serial.print("RAW X: ");
        // Serial.print(touch.RawX());
        // Serial.print(" RAW Y: ");
        // Serial.println(touch.RawY());
        // Serial.print("X: ");
        // Serial.print(touch.X());
        // Serial.print(" Y: ");
        // Serial.println(touch.Y());
    }
    delay(20);
}
