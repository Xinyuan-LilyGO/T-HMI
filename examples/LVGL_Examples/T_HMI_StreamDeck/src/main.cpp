#include <Arduino.h>
#include <SPI.h>
#include <cstdlib>
#include <esp_err.h>
#include <esp_heap_caps.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_vendor.h>
#include <lvgl.h>

#include "../lib/arduino_xpt2046_library/src/xpt2046.h"
#include "backlight.h"
#include "config.h"
#include "hid_manager.h"
#include "ui/ui_manager.h"

namespace {

// The panel is natively 240x320. MADCTL rotates it to 320x240 landscape.
constexpr int PANEL_WIDTH = 240;
constexpr int PANEL_HEIGHT = 320;
constexpr int LCD_WIDTH = 320;
constexpr int LCD_HEIGHT = 240;

constexpr int LCD_PIXEL_CLOCK_HZ = 5 * 1000 * 1000;
constexpr uint8_t TOUCH_ROTATION = 1;
// LilyGo's factory landscape mode is MY | MV (0xA0). 0x60 is the same
// landscape rotated 180 degrees, which makes both the image and touch wrong.
constexpr uint8_t LCD_MADCTL_LANDSCAPE = 0xA0;

constexpr int PWR_EN_PIN = 10;
constexpr int PWR_ON_PIN = 14;
constexpr int LCD_DATA_PINS[] = {48, 47, 39, 40, 41, 42, 45, 46};
constexpr int LCD_PCLK_PIN = 8;
constexpr int LCD_CS_PIN = 6;
constexpr int LCD_DC_PIN = 7;
constexpr int LCD_RST_PIN = -1;
constexpr int LCD_BK_LIGHT_PIN = 38;
constexpr int TOUCH_SCLK_PIN = 1;
constexpr int TOUCH_MISO_PIN = 4;
constexpr int TOUCH_MOSI_PIN = 3;
constexpr int TOUCH_CS_PIN = 2;
constexpr int TOUCH_IRQ_PIN = 9;

constexpr size_t DRAW_BUFFER_LINES = 40;

struct LcdInitCommand {
    uint8_t command;
    uint8_t data[16];
    uint8_t length;
    uint16_t delay_ms;
};

// ST7789 init sequence from the LilyGo T-HMI LVGL example, with landscape
// MADCTL and datasheet-correct gamma lengths (14 bytes, not 16).
const LcdInitCommand LCD_INIT_COMMANDS[] = {
    {0x01, {}, 0, 100}, {0x11, {}, 0, 120},
    {0x36, {LCD_MADCTL_LANDSCAPE}, 1, 0},
    {0x3A, {0x05}, 1, 0},
    {0xB2, {0x0C, 0x0C, 0x00, 0x33, 0x33}, 5, 0},
    {0xB7, {0x35}, 1, 0},
    {0xBB, {0x28}, 1, 0},
    {0xC0, {0x2C}, 1, 0},
    {0xC2, {0x01}, 1, 0},
    {0xC3, {0x0B}, 1, 0},
    {0xC4, {0x20}, 1, 0},
    {0xC6, {0x0F}, 1, 0},
    {0xD0, {0xA4, 0xA1}, 2, 0},
    {0xE0, {0xD0, 0x01, 0x08, 0x0F, 0x11, 0x2A, 0x36, 0x55, 0x44, 0x3A, 0x0B, 0x06, 0x11, 0x20}, 14, 0},
    {0xE1, {0xD0, 0x02, 0x07, 0x0A, 0x0B, 0x18, 0x34, 0x43, 0x4A, 0x2B, 0x1B, 0x1C, 0x22, 0x1F}, 14, 0},
    {0x29, {}, 0, 0},
    {0x2C, {}, 0, 0},
};

XPT2046 touch(SPI, TOUCH_CS_PIN, TOUCH_IRQ_PIN);
esp_lcd_panel_handle_t panel_handle;
esp_lcd_panel_io_handle_t panel_io_handle;

lv_disp_draw_buf_t draw_buffer;
lv_disp_drv_t display_driver;

bool lcd_color_done(esp_lcd_panel_io_handle_t, esp_lcd_panel_io_event_data_t *, void *user_ctx) {
    auto *driver = static_cast<lv_disp_drv_t *>(user_ctx);
    if (driver) lv_disp_flush_ready(driver);
    return false;
}

void lcd_flush(lv_disp_drv_t *driver, const lv_area_t *area, lv_color_t *color_map) {
    auto panel = static_cast<esp_lcd_panel_handle_t>(driver->user_data);
    const esp_err_t err = esp_lcd_panel_draw_bitmap(panel, area->x1, area->y1, area->x2 + 1, area->y2 + 1,
                                                    color_map);
    if (err != ESP_OK) {
        // Without this the display would wait forever for the DMA callback.
        Serial.printf("[LCD] draw_bitmap failed: %s\n", esp_err_to_name(err));
        lv_disp_flush_ready(driver);
    }
}

void touch_read(lv_indev_drv_t *, lv_indev_data_t *data) {
    static bool was_pressed = false;
    const bool pressed = touch.pressed();
    if (pressed != was_pressed && pressed) {
        Serial.printf("[TOUCH] x=%u y=%u raw=%u,%u\n", touch.X(), touch.Y(), touch.RawX(), touch.RawY());
    }
    was_pressed = pressed;
    if (pressed) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch.X();
        data->point.y = touch.Y();
        ui::notify_activity();
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

lv_color_t *allocate_draw_buffer(size_t bytes) {
    auto *buffer = static_cast<lv_color_t *>(heap_caps_malloc(bytes, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL));
    if (buffer == nullptr) {
        buffer = static_cast<lv_color_t *>(heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    }
    return buffer;
}

void initialize_display() {
    pinMode(PWR_ON_PIN, OUTPUT);
    digitalWrite(PWR_ON_PIN, HIGH);
    pinMode(PWR_EN_PIN, OUTPUT);
    digitalWrite(PWR_EN_PIN, HIGH);
    backlight::init(LCD_BK_LIGHT_PIN);
    delay(200);

    esp_lcd_i80_bus_handle_t bus_handle = nullptr;
    esp_lcd_i80_bus_config_t bus_config = {};
    bus_config.dc_gpio_num = LCD_DC_PIN;
    bus_config.wr_gpio_num = LCD_PCLK_PIN;
    for (size_t i = 0; i < sizeof(LCD_DATA_PINS) / sizeof(LCD_DATA_PINS[0]); ++i) {
        bus_config.data_gpio_nums[i] = LCD_DATA_PINS[i];
    }
    bus_config.bus_width = 8;
    bus_config.max_transfer_bytes = LCD_WIDTH * LCD_HEIGHT * sizeof(uint16_t);
    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &bus_handle));

    esp_lcd_panel_io_i80_config_t io_config = {};
    io_config.cs_gpio_num = LCD_CS_PIN;
    io_config.pclk_hz = LCD_PIXEL_CLOCK_HZ;
    io_config.trans_queue_depth = 10;
    io_config.on_color_trans_done = lcd_color_done;
    io_config.user_ctx = &display_driver;
    io_config.lcd_cmd_bits = 8;
    io_config.lcd_param_bits = 8;
    io_config.dc_levels.dc_idle_level = 0;
    io_config.dc_levels.dc_cmd_level = 0;
    io_config.dc_levels.dc_dummy_level = 0;
    io_config.dc_levels.dc_data_level = 1;
    io_config.flags.swap_color_bytes = 1;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(bus_handle, &io_config, &panel_io_handle));

    esp_lcd_panel_dev_config_t panel_config = {};
    panel_config.reset_gpio_num = LCD_RST_PIN;
    panel_config.color_space = ESP_LCD_COLOR_SPACE_RGB;
    panel_config.bits_per_pixel = 16;
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(panel_io_handle, &panel_config, &panel_handle));

    for (const auto &command : LCD_INIT_COMMANDS) {
        ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(panel_io_handle, command.command, command.data,
                                                  command.length));
        if (command.delay_ms) delay(command.delay_ms);
    }
}

void initialize_touch() {
    SPI.begin(TOUCH_SCLK_PIN, TOUCH_MISO_PIN, TOUCH_MOSI_PIN);
    // Calibration is expressed in native portrait coordinates; setRotation()
    // maps it to the landscape display.
    touch.begin(PANEL_WIDTH, PANEL_HEIGHT);
    touch.setCal(1788, 285, 1877, 311, PANEL_WIDTH, PANEL_HEIGHT);
    touch.setRotation(TOUCH_ROTATION);
}

void initialize_lvgl() {
    lv_init();

    const size_t buffer_pixels = LCD_WIDTH * DRAW_BUFFER_LINES;
    const size_t buffer_bytes = buffer_pixels * sizeof(lv_color_t);
    lv_color_t *buffer1 = allocate_draw_buffer(buffer_bytes);
    lv_color_t *buffer2 = allocate_draw_buffer(buffer_bytes);
    if (buffer1 == nullptr || buffer2 == nullptr) {
        Serial.println("[LVGL] draw buffer allocation failed");
        std::abort();
    }

    lv_disp_draw_buf_init(&draw_buffer, buffer1, buffer2, buffer_pixels);

    lv_disp_drv_init(&display_driver);
    display_driver.hor_res = LCD_WIDTH;
    display_driver.ver_res = LCD_HEIGHT;
    display_driver.flush_cb = lcd_flush;
    display_driver.draw_buf = &draw_buffer;
    display_driver.user_data = panel_handle;
    if (lv_disp_drv_register(&display_driver) == nullptr) {
        Serial.println("[LVGL] display registration failed");
        std::abort();
    }

    static lv_indev_drv_t input_driver;
    lv_indev_drv_init(&input_driver);
    input_driver.type = LV_INDEV_TYPE_POINTER;
    input_driver.read_cb = touch_read;
    if (lv_indev_drv_register(&input_driver) == nullptr) {
        Serial.println("[LVGL] input registration failed");
        std::abort();
    }
}

} // namespace

void setup() {
    Serial.begin(115200);
    if (!config_is_valid()) {
        Serial.println("[config] invalid button action; check include/config.h");
        std::abort();
    }
    initialize_touch();
    initialize_display();
    initialize_lvgl();
    HID.begin();
    ui::build();
    ui::show_splash(SPLASH_DURATION_MS);
}

void loop() {
    lv_timer_handler();
    delay(5);
}
