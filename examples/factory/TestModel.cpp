#include "TestModel.h"
#include "pins.h"

#include <spiram.h>
#include <Esp.h>
#include <WiFi.h>
#include <SD_MMC.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>


TestModel::TestModel() {

}


TestModel::~TestModel() {

}


void TestModel::init() {
    xTaskCreatePinnedToCore(
        testTask,
        "testTask",
        4096,
        this,
        6,
        &_testTaskHandler,
        1
    );
}


void TestModel::deinit() {
    vTaskDelete(_testTaskHandler);
}


String TestModel::getRamSizeTestResult() {
    String result = "";
    if ((esp_spiram_get_size() / 1024 / 1024) == 8) {
        result = "PASS - (";
        result += (esp_spiram_get_size() / 1024 / 1024);
        result += "MB)";
    } else {
        result = "FAIL - (";
        result += (esp_spiram_get_size());
        result += ")";
    }
    return result;
}


String TestModel::getFlashTestResult() {
    String result = "";
    if ((ESP.getFlashChipSize() / 1024 / 1024) == 16) {
        result = "PASS - (";
        result += (ESP.getFlashChipSize() / 1024 / 1024);
        result += "MB)";
    } else {
        result = "FAIL - (";
        result += (esp_spiram_get_size());
        result += ")";
    }
    return result;
}


String TestModel::getSDTestResult() {
    return _sdTestResult;
}


String TestModel::getWiFiRSSIResult() {
    String result = "";
    if (WiFi.status() == WL_CONNECTED) {
        if (WiFi.RSSI() >= -60) {
            result = "PASS";
        } else {
            result = "FAIL";
        }
        result += " - (RSSI: ";
        result += WiFi.RSSI();
        result += ")";
    } else {
        result = "Testing";
    }
    return result;
}


String TestModel::getWiFiConnectResult() {
    String result = "";
    switch (WiFi.status()) {
        case WL_IDLE_STATUS:
            result = "FAIL - (IDLE)";
        break;

        case WL_NO_SSID_AVAIL:
            result = "FAIL - (No SSID Avail)";
        break;

        case WL_SCAN_COMPLETED:
            result = "FAIL - (Scan Completed)";
        break;

        case WL_CONNECTED:
            result = "PASS - (";
            result += WiFi.localIP().toString();
            result += ")";
        break;

        case WL_CONNECT_FAILED:
            result = "FAIL - (Connect Failed)";
        break;

        case WL_CONNECTION_LOST:
            result = "FAIL - (Connection Lost)";
        break;

        case WL_DISCONNECTED:
            result = "FAIL - (Disconnected)";
        break;

        default: break;
    }
    return result;
}


void TestModel::testTask(void *param) {
    TestModel *model = (TestModel *)param;
    String result = "";
    bool sdStatus = false;

    SD_MMC.setPins(SD_SCLK_PIN, SD_MOSI_PIN, SD_MISO_PIN);

    while (1) {
        if (sdStatus == false) {
            sdStatus = SD_MMC.begin("/sdcard", true);
            if (!sdStatus) {
                Serial.println("SD init failed");
                Serial.println("No detected SdCard");
                result = "FAIL";
            } else {
                Serial.println("SD init success");
                Serial.printf(
                    "Detected SdCard insert: %.2f GB\r\n",
                    SD_MMC.cardSize() / 1024.0 / 1024.0 / 1024.0
                );
                result = "PASS - (";
                result += (SD_MMC.cardSize() / 1024 / 1024 / 1024);
                result += "GB)";
            }
            if (model->_sdTestResult != result) {
                model->_sdTestResult = result;
            }
        }
        delay(3000);
    }

    vTaskDelete(NULL);
}
