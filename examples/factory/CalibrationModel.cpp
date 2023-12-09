#include "CalibrationModel.h"

#if defined __has_include
#if __has_include("lvgl.h")
#include "lvgl.h"
#elif __has_include("lvgl/lvgl.h")
#include "lvgl/lvgl.h"
#else
#include "lvgl.h"
#endif
#else
#include "lvgl.h"
#endif

#include <ArduinoJson.h>

#include <nvs_flash.h>
#include <nvs.h>

#include <Arduino.h>



CalibrationModel::CalibrationModel() {

}


CalibrationModel::~CalibrationModel() {

}


TouchCalibration CalibrationModel::get(int32_t index) {
    return _rawData[index];
}


void CalibrationModel::set(TouchCalibration data, int32_t index) {
    _rawData[index] = data;
}


void CalibrationModel::init() {
    _calibrated = readFromNVS();
}


bool CalibrationModel::readFromNVS() {
    nvs_handle_t data_handle;
    size_t required_size = 0;
    esp_err_t err;

    err = nvs_open("touch", NVS_READWRITE, &data_handle);
    if (err != ESP_OK) {
        Serial.println(F("nvs open error"));
        goto OUT;
    }

    err = nvs_get_blob(data_handle, "data", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        Serial.println(F("nvs open err"));
        goto OUT;
    }

    if (required_size == 0) {
        Serial.println(F("Nothing saved yet"));
        goto OUT;
    } else {
        uint8_t *str = (uint8_t *)malloc(required_size);
        if (!str) {
            Serial.println(F("malloc failed"));
            goto OUT;
        }
        err = nvs_get_blob(data_handle, "data", str, &required_size);
        if (err != ESP_OK) {
            Serial.println(F("Unable to get blob value"));
            free(str);
            goto OUT;
        }
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, str);
        if (error) {
            Serial.println(F("json parsing error"));
            goto OUT;
        }
        // Serial.printf("%s\n", str);
        _rawData[0].rawX = doc["point_1"]["raw_x"].as<uint16_t>();
        _rawData[0].rawY = doc["point_1"]["raw_Y"].as<uint16_t>();
        _rawData[1].rawX = doc["point_2"]["raw_x"].as<uint16_t>();
        _rawData[1].rawY = doc["point_2"]["raw_Y"].as<uint16_t>();
        _rawData[2].rawX = doc["point_3"]["raw_x"].as<uint16_t>();
        _rawData[2].rawY = doc["point_3"]["raw_Y"].as<uint16_t>();
        _rawData[3].rawX = doc["point_4"]["raw_x"].as<uint16_t>();
        _rawData[3].rawY = doc["point_4"]["raw_Y"].as<uint16_t>();
        Serial.println("point_1:");
        Serial.print("  raw_x: ");
        Serial.print(_rawData[0].rawX);
        Serial.print("  raw_y: ");
        Serial.println(_rawData[0].rawY);

        Serial.println("point_2:");
        Serial.print("  raw_x: ");
        Serial.print(_rawData[1].rawX);
        Serial.print("  raw_y: ");
        Serial.println(_rawData[1].rawY);

        Serial.println("point_3:");
        Serial.print("  raw_x: ");
        Serial.print(_rawData[2].rawX);
        Serial.print("  raw_y: ");
        Serial.println(_rawData[2].rawY);

        Serial.println("point_4:");
        Serial.print("  raw_x: ");
        Serial.print(_rawData[3].rawX);
        Serial.print("  raw_y: ");
        Serial.println(_rawData[3].rawY);
        free(str);
    }

    nvs_close(data_handle);
    return true;
OUT:
    nvs_close(data_handle);
    return false;
}


bool CalibrationModel::writeToNVS() {
    nvs_handle_t data_handle;
    esp_err_t err;
    StaticJsonDocument<1024> doc;
    char str[1024] = { 0 };

    err = nvs_open("touch", NVS_READWRITE, &data_handle);
    if (err != ESP_OK) {
        Serial.println(F("nvs open error"));
        return false;
    }

    // doc["type"] = "Resistive touch screen";
    doc["point_1"]["raw_x"] = _rawData[0].rawX;
    doc["point_1"]["raw_Y"] = _rawData[0].rawY;
    doc["point_2"]["raw_x"] = _rawData[1].rawX;
    doc["point_2"]["raw_Y"] = _rawData[1].rawY;
    doc["point_3"]["raw_x"] = _rawData[2].rawX;
    doc["point_3"]["raw_Y"] = _rawData[2].rawY;
    doc["point_4"]["raw_x"] = _rawData[3].rawX;
    doc["point_4"]["raw_Y"] = _rawData[3].rawY;
    serializeJson(doc, str, sizeof(str) - 1);

    err = nvs_set_blob(data_handle, "data", str, strlen(str));
    nvs_close(data_handle);
    if (err != ESP_OK) {
        return false;
    }
    return true;
}


bool CalibrationModel::getStatus() {
    return _calibrated;
}


void CalibrationModel::deinit() {

}


