#include "data.h"
#include "pins.h"
#include "image/logo.h"
#include "image/LilyGoFCC.h"

#include "CalibrationController.h"
#include "TestController.h"
#include "MonitorController.h"
#include "TimerController.h"
#include "SketchpadController.h"
#include "lv_conf.h"
#include <lvgl.h>

#include <xpt2046.h> // https://github.com/liangyingy/arduino_xpt2046_library.git
#include <OneButton.h>

#include <WiFi.h>
#include <SD_MMC.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <Arduino.h>
#include <vector>

#include <sntp.h>
#include <time.h>

#include <nvs_flash.h>
#include <nvs.h>

#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_rgb.h>
#include <esp_lcd_panel_vendor.h>

#define CONFIG_RSSI (-60)

#define NTP_SERVER1                  "pool.ntp.org"
#define NTP_SERVER2                  "time.nist.gov"
#define GMT_OFFSET_SEC               (3600 * 8)
#define DAY_LIGHT_OFFSET_SEC         0

/**
 * Please update the following configuration according to your LCD spec
 */
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ     (10 * 1000 * 1000)
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  (1)
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL (!EXAMPLE_LCD_BK_LIGHT_ON_LEVEL)
#define EXAMPLE_PIN_NUM_DATA0          (LCD_DATA0_PIN)
#define EXAMPLE_PIN_NUM_DATA1          (LCD_DATA1_PIN)
#define EXAMPLE_PIN_NUM_DATA2          (LCD_DATA2_PIN)
#define EXAMPLE_PIN_NUM_DATA3          (LCD_DATA3_PIN)
#define EXAMPLE_PIN_NUM_DATA4          (LCD_DATA4_PIN)
#define EXAMPLE_PIN_NUM_DATA5          (LCD_DATA5_PIN)
#define EXAMPLE_PIN_NUM_DATA6          (LCD_DATA6_PIN)
#define EXAMPLE_PIN_NUM_DATA7          (LCD_DATA7_PIN)
#define EXAMPLE_PIN_NUM_PCLK           (PCLK_PIN)
#define EXAMPLE_PIN_NUM_CS             (CS_PIN)
#define EXAMPLE_PIN_NUM_DC             (DC_PIN)
#define EXAMPLE_PIN_NUM_RST            (RST_PIN)
#define EXAMPLE_PIN_NUM_BK_LIGHT       (BK_LIGHT_PIN)

/**
 * The pixel number in horizontal and vertical
 */
#define EXAMPLE_LCD_H_RES              (240)
#define EXAMPLE_LCD_V_RES              (320)

/**
 * Bit number used to represent command and parameter
 */
#define EXAMPLE_LCD_CMD_BITS           (8)
#define EXAMPLE_LCD_PARAM_BITS         (8)

XPT2046 touch = XPT2046(SPI, TOUCHSCREEN_CS_PIN, TOUCHSCREEN_IRQ_PIN);

CalibrationController calibrationController(touch);
TestController testController;
MonitorController monitorController;
TimerController timerController;
SketchpadController sketchpadController;

OneButton buttonOK(21, false, false);
OneButton buttonNext(BUTTON1_PIN, true, false);

uint8_t currentScreen = 0;

portMUX_TYPE flushLock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;

// Please enter the ssid and password of the wifi
const char *ssid     = "xinyuandianzi";
const char *password = "AA15994823428";

static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
static lv_disp_drv_t disp_drv;      // contains callback functions
static lv_indev_drv_t indev_drv;

void setup()
{
    pinMode(PWR_ON_PIN, OUTPUT);
    digitalWrite(PWR_ON_PIN, HIGH);

    Serial.begin(115200);
    delay(10);
    Serial.println(F("Turn on the main power"));

    Serial.println(F("Power on peripherals, such as the LCD backlight"));
    pinMode(PWR_EN_PIN, OUTPUT);
    digitalWrite(PWR_EN_PIN, HIGH);

    Serial.println(F("Initialize the power button"));
    buttonOK.attachClick(power_off);
    buttonOK.attachLongPressStop(power_off);

    Serial.println(F("Initialize Intel 8080 bus"));
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = EXAMPLE_PIN_NUM_DC,
        .wr_gpio_num = EXAMPLE_PIN_NUM_PCLK,
        .clk_src = LCD_CLK_SRC_PLL160M,
        .data_gpio_nums = {
            EXAMPLE_PIN_NUM_DATA0,
            EXAMPLE_PIN_NUM_DATA1,
            EXAMPLE_PIN_NUM_DATA2,
            EXAMPLE_PIN_NUM_DATA3,
            EXAMPLE_PIN_NUM_DATA4,
            EXAMPLE_PIN_NUM_DATA5,
            EXAMPLE_PIN_NUM_DATA6,
            EXAMPLE_PIN_NUM_DATA7,
        },
        .bus_width = 8,
        .max_transfer_bytes = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * sizeof(uint16_t)
    };
    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = EXAMPLE_PIN_NUM_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 10,
        // .on_color_trans_done = example_notify_lvgl_flush_ready,
        .user_ctx = &disp_drv,
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .flags = {
            .swap_color_bytes = 1,
        }
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    Serial.println(F("Install LCD driver of st7789"));
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    Serial.println(F("Initialize LCD driver of st7789"));
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_mirror(panel_handle, false, true);
    esp_lcd_panel_swap_xy(panel_handle, true);

    Serial.println(F("Turn on the LCD backlight"));
    pinMode(EXAMPLE_PIN_NUM_BK_LIGHT, OUTPUT);
    digitalWrite(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);

    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, 320, 240, gImage_LilyGoFCC);
    delay(3000);

    Serial.println(F("Initialize touch screen calibration data"));
    data_init();
    touch_calibration_t calibrationData[4];
    bool calibrated = data_read(calibrationData);

    Serial.println(F("Initialize the touch screen"));
    SPI.begin(TOUCHSCREEN_SCLK_PIN, TOUCHSCREEN_MISO_PIN, TOUCHSCREEN_MOSI_PIN);
    touch.begin(240, 320);
    if (calibrated) {
        touch.setCal(
            calibrationData[0].rawX,
            calibrationData[2].rawX,
            calibrationData[0].rawY,
            calibrationData[2].rawY,
            240,
            320
        );
    } else {
        touch.setCal(285, 1788, 311, 1877, 240, 320);
    }
    touch.setRotation(3);

    Serial.println(F("Initialize lvgl"));
    lv_init();
    // alloc draw buffers used by LVGL from PSRAM
    lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(
                           EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * sizeof(lv_color_t),
                           MALLOC_CAP_SPIRAM
                       );
    if (!buf1) {
        Serial.println(F("lvgl buffer 1 application failed"));
        Serial.println(F("system restarting..."));
        delay(100);
        esp_restart();
    }
    lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(
                           EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * sizeof(lv_color_t),
                           MALLOC_CAP_SPIRAM
                       );
    if (buf2) {
        Serial.println(F("lvgl uses double buffer"));
        lv_disp_draw_buf_init(
            &disp_buf,
            buf1,
            // buf2,
            NULL,
            EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES
        );
    } else {
        Serial.println(F("lvgl buffer 2 application failed"));
        Serial.println(F("use single buffer"));
        lv_disp_draw_buf_init(
            &disp_buf,
            buf1,
            NULL,
            EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES
        );
    }

    Serial.println(F("Register display driver to lvgl"));
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_V_RES;
    disp_drv.ver_res = EXAMPLE_LCD_H_RES;
    disp_drv.flush_cb = example_lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
    disp_drv.full_refresh = 1;
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    if (calibrated) {
        Serial.println(F("Register input driver to lvgl"));
        lv_indev_drv_init(&indev_drv);
        indev_drv.type = LV_INDEV_TYPE_POINTER;
        indev_drv.read_cb = lv_touchpad_read;
        indev_drv.user_data = &touch;
        lv_indev_t *indev = lv_indev_drv_register(&indev_drv);
    }

    Serial.println(F("run lvgl task"));
    xTaskCreatePinnedToCore(lvglTask, "lvglTask", 4096, NULL, 6, NULL, 1);

    wifiInit();

    calibrationController.init();
    testController.init();
    timerController.init();
    monitorController.init();
    sketchpadController.init();

    if (calibrated) {
        testController.onViewLoad();
        currentScreen = 1;
    } else {
        calibrationController.onViewLoad();
        currentScreen = 0;
    }
    buttonNext.attachClick(changeScreen);
}


void loop()
{
    lv_timer_handler();
    delay(10);
}


static void power_off()
{
    Serial.println(F("shutting down..."));
    delay(100);
    digitalWrite(PWR_ON_PIN, LOW);
}


static void changeScreen()
{
    switch ((++currentScreen) % 5) {
    case 0:
        sketchpadController.onViewDisappear();
        calibrationController.onViewLoad();
        break;

    case 1:
        calibrationController.onViewDisappear();
        testController.onViewLoad();
        break;

    case 2:
        testController.onViewDisappear();
        timerController.onViewLoad();
        break;

    case 3:
        timerController.onViewDisappear();
        monitorController.onViewLoad();
        break;

    case 4:
        monitorController.onViewDisappear();
        sketchpadController.onViewLoad();
        break;

    default: break;
    }
}


static void example_lvgl_flush_cb(
    lv_disp_drv_t *drv,
    const lv_area_t *area,
    lv_color_t *color_map
)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
    // taskENTER_CRITICAL(&flushLock);
    esp_lcd_panel_draw_bitmap(
        panel_handle,
        area->x1,
        area->y1,
        area->x2 + 1,
        area->y2 + 1,
        color_map
    );
    // taskEXIT_CRITICAL(&flushLock);
    lv_disp_flush_ready(drv);
}


static void lv_touchpad_read(
    lv_indev_drv_t *indev_driver,
    lv_indev_data_t *data
)
{
    XPT2046 *touch = (XPT2046 *)indev_driver->user_data;
    if (touch->pressed()) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch->X();
        data->point.y = touch->Y();
        // Serial.print("X: ");
        // Serial.print(data->point.x);
        // Serial.print(", Y: ");
        // Serial.println(data->point.y);
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}


static void lvglTask(void *param)
{
    while (1) {
        buttonOK.tick();
        buttonNext.tick();
        delay(10);
    }
    vTaskDelete(NULL);
}


#if 0
void calibrationTask(void *param)
{
    uint8_t point = 0;
    bool flag = false;
    uint32_t last = 0;
    std::vector<touch_calibration_t> raw_data;

    lv_page_calibration_init();
    lv_page_calibration_load();

    while (1) {
        if (touch.pressed()) {
            flag = true;
            last = millis();
        }
        if (flag && millis() - last > 200) {
            flag = false;
            lv_event_send(ui_Screen1, LV_EVENT_CLICKED, NULL);
            break;
        }
        delay(10);
    }

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

        if (flag && millis() - last > 500) {
            flag = false;
            uint32_t tmpX = 0;
            uint32_t tmpY = 0;

            for (size_t i = 0; i < raw_data.size(); i++) {
                tmpX += raw_data[i].rawX;
                tmpY += raw_data[i].rawY;
            }
            calibration_data[point].rawX = tmpX / raw_data.size();
            calibration_data[point].rawY = tmpY / raw_data.size();
            raw_data.clear();

            point++;
            if (point == 1) {
                lv_event_send(ui_Screen1_Button1, LV_EVENT_CLICKED, NULL);
            } else if (point == 2) {
                lv_event_send(ui_Screen1_Button2, LV_EVENT_CLICKED, NULL);
            } else if (point == 3) {
                lv_event_send(ui_Screen1_Button3, LV_EVENT_CLICKED, NULL);
            } else if (point == 4) {
                data_write(calibration_data);
                lv_event_send(ui_Screen1_Button4, LV_EVENT_CLICKED, NULL);
                delay(100);
                touch.setCal(
                    calibration_data[0].rawX, // Raw xmin
                    calibration_data[2].rawX, // Raw xmax
                    calibration_data[0].rawY, // Raw ymin
                    calibration_data[2].rawY, // Raw ymax
                    240,                      // width
                    320                       // height
                );
                touch.setRotation(3);
                calibrated = true;
                break ;
            }
        }
        delay(10);
    }

    vTaskDelete(NULL);
}



void testTask(void *param)
{
    String result = "";
    bool sdStatus = false;

    if (param != NULL) {
        ((void(*)(void))param)();
    }

    lv_test_page_init();
    lv_test_page_load();

    SD_MMC.setPins(SD_SCLK_PIN, SD_MOSI_PIN, SD_MISO_PIN);

    // psram test
    if ((esp_spiram_get_size() / 1024 / 1024) == 8) {
        result = "PASS - (";
        result += (esp_spiram_get_size() / 1024 / 1024);
        result += "MB)";
    } else {
        result = "FAIL - (";
        result += (esp_spiram_get_size());
        result += ")";
    }
    lv_msg_send(MSG_NEW_PSRAM, result.c_str());
    delay(10);

    // flash test

    if ((ESP.getFlashChipSize() / 1024 / 1024) == 16) {
        result = "PASS - (";
        result += (ESP.getFlashChipSize() / 1024 / 1024);
        result += "MB)";
    } else {
        result = "FAIL - (";
        result += (esp_spiram_get_size());
        result += ")";
    }
    lv_msg_send(MSG_NEW_FLASH, result.c_str());

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
            lv_msg_send(MSG_NEW_SD, result.c_str());
        }

        if (WiFi.RSSI() >= CONFIG_RSSI) {
            result = "PASS";
        } else {
            result = "FAIL";
        }
        result += " - (RSSI: ";
        result += WiFi.RSSI();
        result += ")";
        lv_msg_send(MSG_NEW_RSSI, result.c_str());

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
        lv_msg_send(MSG_NEW_WIFI, result.c_str());

        delay(3000);
    }

    vTaskDelete(NULL);
}


void timerTask(void *param)
{
    struct tm timeinfo;

    sntp_servermode_dhcp(1);
    configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);

    lv_timer_page_init();
    lv_timer_page_load();

    while (1) {
        if (getLocalTime(&timeinfo)) {
            lv_msg_send(MSG_NEW_HOUR, &timeinfo.tm_hour);
            lv_msg_send(MSG_NEW_MIN, &timeinfo.tm_min);
        }
        delay(1000);
    }
    lv_timer_page_deinit();
    vTaskDelete(NULL);
}


void monitorTask(void *param)
{
    int32_t temperature = 0;
    int32_t humidity = 0;

    lv_monitor_page_init();
    lv_monitor_page_load();

    while (1) {
        temperature = random(0, 40);
        humidity = random(50, 100);
        lv_msg_send(MSG_NEW_TEMPERATURE, &temperature);
        lv_msg_send(MSG_NEW_HUMIDITY, &humidity);
        delay(500);
    }
    lv_monitor_page_deinit();
    vTaskDelete(NULL);
}
#endif


void wifiInit(void)
{
    WiFi.disconnect(true);
    delay(100);

    Serial.println(F("WiFi: Set mode to STA"));

    WiFi.mode(WIFI_AP_STA);
    WiFi.onEvent(WiFiEvent);
    WiFi.begin(ssid, password);

    Serial.println(F("Connecting to WiFi"));
}


void WiFiEvent(WiFiEvent_t event)
{
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch (event) {
    case ARDUINO_EVENT_WIFI_READY:
        Serial.println("WiFi interface ready");
        break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:
        Serial.println("Completed scan for access points");
        break;
    case ARDUINO_EVENT_WIFI_STA_START:
        Serial.println("WiFi client started");
        break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
        Serial.println("WiFi clients stopped");
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("Connected to access point");
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        WiFi.begin(ssid, password);
        Serial.println("Disconnected from WiFi access point");
        break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
        Serial.println("Authentication mode of access point has changed");
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.print("Obtained IP address: ");
        Serial.println(WiFi.localIP());
        break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        Serial.println("Lost IP address and IP address is reset to 0");
        break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:
        Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
        break;
    case ARDUINO_EVENT_WPS_ER_FAILED:
        Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
        break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:
        Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
        break;
    case ARDUINO_EVENT_WPS_ER_PIN:
        Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
        break;
    case ARDUINO_EVENT_WIFI_AP_START:
        Serial.println("WiFi access point started");
        break;
    case ARDUINO_EVENT_WIFI_AP_STOP:
        Serial.println("WiFi access point  stopped");
        break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
        Serial.println("Client connected");
        break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
        Serial.println("Client disconnected");
        break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
        Serial.println("Assigned IP address to client");
        break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
        Serial.println("Received probe request");
        break;
    case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
        Serial.println("AP IPv6 is preferred");
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
        Serial.println("STA IPv6 is preferred");
        break;
    case ARDUINO_EVENT_ETH_GOT_IP6:
        Serial.println("Ethernet IPv6 is preferred");
        break;
    case ARDUINO_EVENT_ETH_START:
        Serial.println("Ethernet started");
        break;
    case ARDUINO_EVENT_ETH_STOP:
        Serial.println("Ethernet stopped");
        break;
    case ARDUINO_EVENT_ETH_CONNECTED:
        Serial.println("Ethernet connected");
        break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
        Serial.println("Ethernet disconnected");
        break;
    case ARDUINO_EVENT_ETH_GOT_IP:
        Serial.println("Obtained IP address");
        break;
    default: break;
    }
}