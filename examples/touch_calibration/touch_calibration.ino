#include "data.h"
#include "pins.h"

#include <xpt2046.h> // https://github.com/liangyingy/arduino_xpt2046_library.git
#include <TFT_eSPI.h>

#include <SPI.h>
#include <Arduino.h>
#include <vector>


XPT2046 touch = XPT2046(SPI, TOUCHSCREEN_CS_PIN, TOUCHSCREEN_IRQ_PIN);
TFT_eSPI tft = TFT_eSPI();

touch_calibration_t calibration_data[4];
std::vector<touch_calibration_t> raw_data;


void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(PWR_EN_PIN, OUTPUT);
    digitalWrite(PWR_EN_PIN, HIGH);

    data_init();
    data_read(calibration_data);
    SPI.begin(TOUCHSCREEN_SCLK_PIN, TOUCHSCREEN_MISO_PIN, TOUCHSCREEN_MOSI_PIN);
    touch.begin(240, 320);
    touch.setCal(1903, 253, 1916, 185, 240, 320); // Raw xmin, xmax, ymin, ymax, width, height
    touch.setRotation(0);
    tft.begin();
    tft.setRotation(0);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);
    tft.drawCircle(10,  10,  10, TFT_WHITE);
    tft.drawString("Please click the circle", 40, 160, 2);
    // tft.drawCircle(229, 10,  10, TFT_WHITE);
    // tft.drawCircle(229, 309, 10, TFT_WHITE);
    // tft.drawCircle(10,  309, 10, TFT_WHITE);
    xTaskCreatePinnedToCore(calibrationTask, "calibrationTask", 4096, NULL, 6, NULL, 1);
}

uint8_t point = 0;
// int32_t count;
bool flag = false;
uint32_t last = 0;

void loop() {
    delay(20);
}


void calibrationTask(void *param) {
    while (1) {
        if (touch.pressed()) {
            flag = true;
            last = millis();
            // count++;
            Serial.print("RAW X: ");
            Serial.print(touch.RawX());
            Serial.print(" RAW Y: ");
            Serial.println(touch.RawY());
            Serial.print("X: ");
            Serial.print(touch.X());
            Serial.print(" Y: ");
            Serial.println(touch.Y());
            raw_data.push_back((touch_calibration_t) {
                .rawX = touch.RawX(),
                .rawY = touch.RawY()
            });
        }

        if (flag && millis() - last > 200) {
            flag = false;
            uint32_t tmpX = 0;
            uint32_t tmpY = 0;

            for (size_t i = 0; i < raw_data.size(); i++) {
                tmpX += raw_data[i].rawX;
                tmpY += raw_data[i].rawY;
            }
            calibration_data[point].rawX = tmpX / raw_data.size();
            calibration_data[point].rawY = tmpY / raw_data.size();
            // count = 0;
            raw_data.clear();
            if (++point > 3) {
                data_write(calibration_data);
                tft.fillScreen(TFT_BLACK);
                tft.drawString("calibration complete", 40, 160, 2);
                delay(1000);
                break ;
            }
            tft.fillScreen(TFT_BLACK);
            if (point == 1) {
                tft.drawCircle(229, 10,  10, TFT_WHITE);
                tft.drawString("Please click the circle", 40, 160, 2);
            } else if (point == 2) {
                tft.drawCircle(229, 309, 10, TFT_WHITE);
                tft.drawString("Please click the circle", 40, 160, 2);
            } else if (point == 3) {
                tft.drawCircle(10,  309, 10, TFT_WHITE);
                tft.drawString("Please click the circle", 40, 160, 2);
            }
        }
    }
    vTaskDelete(NULL);
}