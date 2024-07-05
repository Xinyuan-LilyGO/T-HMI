#include "pins.h"
#include "Arduino.h"
#include "TFT_eSPI.h"/* Please use the TFT library provided in the library. */
#include "logo.h"

TFT_eSPI tft = TFT_eSPI();
#define WAIT 1000
unsigned long targetTime = 0; // Used for testing draw times

void setBrightness(uint8_t value)
{
    static uint8_t steps = 16;
    static uint8_t _brightness = 0;

    if (_brightness == value) {
        return;
    }

    if (value > 16) {
        value = 16;
    }
    if (value == 0) {
        digitalWrite(BK_LIGHT_PIN, 0);
        delay(3);
        _brightness = 0;
        return;
    }
    if (_brightness == 0) {
        digitalWrite(BK_LIGHT_PIN, 1);
        _brightness = steps;
        delayMicroseconds(30);
    }
    int from = steps - _brightness;
    int to = steps - value;
    int num = (steps + to - from) % steps;
    for (int i = 0; i < num; i++) {
        digitalWrite(BK_LIGHT_PIN, 0);
        digitalWrite(BK_LIGHT_PIN, 1);
    }
    _brightness = value;
}

void setup()
{
    pinMode(PWR_EN_PIN, OUTPUT);
    digitalWrite(PWR_EN_PIN, HIGH);

    Serial.begin(115200);
    Serial.println("Hello T-HMI");


    tft.begin();
    tft.setRotation(0);
    tft.setSwapBytes(true);
    tft.pushImage(0, 0, 240, 320, (uint16_t *)gImage_logo);

    // Set backlight level, range 0 ~ 16
    setBrightness(16);

    delay(3000);
}


uint8_t rotation = 0;

void loop()
{

    tft.setRotation(rotation);

    targetTime = millis();

    // First we test them with a background colour set
    tft.setTextSize(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);

    tft.drawString(" !\"#$%&'()*+,-./0123456", 0, 0, 2);
    tft.drawString("789:;<=>?@ABCDEFGHIJKL", 0, 16, 2);
    tft.drawString("MNOPQRSTUVWXYZ[\\]^_`", 0, 32, 2);
    tft.drawString("abcdefghijklmnopqrstuvw", 0, 48, 2);
    int xpos = 0;
    xpos += tft.drawString("xyz{|}~", 0, 64, 2);
    tft.drawChar(127, xpos, 64, 2);
    delay(WAIT);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);

    tft.drawString(" !\"#$%&'()*+,-.", 0, 0, 4);
    tft.drawString("/0123456789:;", 0, 26, 4);
    tft.drawString("<=>?@ABCDE", 0, 52, 4);
    tft.drawString("FGHIJKLMNO", 0, 78, 4);
    tft.drawString("PQRSTUVWX", 0, 104, 4);
    delay(WAIT);

    tft.fillScreen(TFT_BLACK);
    tft.drawString("YZ[\\]^_`abc", 0, 0, 4);
    tft.drawString("defghijklmno", 0, 26, 4);
    tft.drawString("pqrstuvwxyz", 0, 52, 4);
    xpos = 0;
    xpos += tft.drawString("{|}~", 0, 78, 4);
    tft.drawChar(127, xpos, 78, 4);
    delay(WAIT);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);

    tft.drawString("012345", 0, 0, 6);
    tft.drawString("6789", 0, 40, 6);
    tft.drawString("apm-:.", 0, 80, 6);
    delay(WAIT);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED, TFT_BLACK);

    tft.drawString("0123", 0, 0, 7);
    tft.drawString("4567", 0, 60, 7);
    delay(WAIT);

    tft.fillScreen(TFT_BLACK);
    tft.drawString("890:.", 0, 0, 7);
    tft.drawString("", 0, 60, 7);
    delay(WAIT);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);

    tft.drawString("01", 0, 0, 8);
    delay(WAIT);

    tft.drawString("23", 0, 0, 8);
    delay(WAIT);

    tft.drawString("45", 0, 0, 8);
    delay(WAIT);

    tft.drawString("67", 0, 0, 8);
    delay(WAIT);

    tft.drawString("89", 0, 0, 8);
    delay(WAIT);

    tft.drawString("0:.", 0, 0, 8);
    delay(WAIT);

    tft.setTextColor(TFT_MAGENTA);
    tft.drawNumber(millis() - targetTime, 0, 100, 4);
    delay(4000);

    // Now test them with transparent background
    targetTime = millis();

    tft.setTextSize(1);
    tft.fillScreen(TFT_BROWN);
    tft.setTextColor(TFT_GREEN);

    tft.drawString(" !\"#$%&'()*+,-./0123456", 0, 0, 2);
    tft.drawString("789:;<=>?@ABCDEFGHIJKL", 0, 16, 2);
    tft.drawString("MNOPQRSTUVWXYZ[\\]^_`", 0, 32, 2);
    tft.drawString("abcdefghijklmnopqrstuvw", 0, 48, 2);
    xpos = 0;
    xpos += tft.drawString("xyz{|}~", 0, 64, 2);
    tft.drawChar(127, xpos, 64, 2);
    delay(WAIT);

    tft.fillScreen(TFT_BROWN);
    tft.setTextColor(TFT_GREEN);

    tft.drawString(" !\"#$%&'()*+,-.", 0, 0, 4);
    tft.drawString("/0123456789:;", 0, 26, 4);
    tft.drawString("<=>?@ABCDE", 0, 52, 4);
    tft.drawString("FGHIJKLMNO", 0, 78, 4);
    tft.drawString("PQRSTUVWX", 0, 104, 4);

    delay(WAIT);
    tft.fillScreen(TFT_BROWN);
    tft.drawString("YZ[\\]^_`abc", 0, 0, 4);
    tft.drawString("defghijklmno", 0, 26, 4);
    tft.drawString("pqrstuvwxyz", 0, 52, 4);
    xpos = 0;
    xpos += tft.drawString("{|}~", 0, 78, 4);
    tft.drawChar(127, xpos, 78, 4);
    delay(WAIT);

    tft.fillScreen(TFT_BROWN);
    tft.setTextColor(TFT_BLUE);

    tft.drawString("012345", 0, 0, 6);
    tft.drawString("6789", 0, 40, 6);
    tft.drawString("apm-:.", 0, 80, 6);
    delay(WAIT);

    tft.fillScreen(TFT_BROWN);
    tft.setTextColor(TFT_RED);

    tft.drawString("0123", 0, 0, 7);
    tft.drawString("4567", 0, 60, 7);
    delay(WAIT);

    tft.fillScreen(TFT_BROWN);
    tft.drawString("890:.", 0, 0, 7);
    tft.drawString("", 0, 60, 7);
    delay(WAIT);

    tft.fillScreen(TFT_BROWN);
    tft.setTextColor(TFT_YELLOW);

    tft.drawString("0123", 0, 0, 8);
    delay(WAIT);

    tft.fillScreen(TFT_BROWN);
    tft.drawString("4567", 0, 0, 8);
    delay(WAIT);

    tft.fillScreen(TFT_BROWN);
    tft.drawString("890:.", 0, 0, 8);
    delay(WAIT);

    tft.setTextColor(TFT_MAGENTA);

    tft.drawNumber(millis() - targetTime, 0, 100, 4);
    delay(4000);

    rotation++;
    rotation %= 4;
}