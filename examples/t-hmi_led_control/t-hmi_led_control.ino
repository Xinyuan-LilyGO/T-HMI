#include "image/logo.h"
#include "init_code.h"
#include "pins.h"

#if __has_include("data.h")
#include "data.h"
#define USE_CALIBRATION_DATA 1
#endif

#include "xpt2046.h"

#include "lv_conf.h"
#include "lvgl.h"

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_vendor.h"

#include <SPI.h>
#include <Arduino.h>

/**
 * Please update the following configuration according to your LCD spec
 */
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ     (5 * 1000 * 1000)
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

// SPIClass SPI;
XPT2046 touch(SPI, TOUCHSCREEN_CS_PIN, TOUCHSCREEN_IRQ_PIN);
// static bool touch_pin_get_int = false;

#if USE_CALIBRATION_DATA
touch_calibration_t calibration_data[4];
#endif

void print_chip_info(void) {
    Serial.print("Chip: ");
    Serial.println(ESP.getChipModel());
    Serial.print("ChipRevision: ");
    Serial.println(ESP.getChipRevision());
    Serial.print("Psram size: ");
    Serial.print(ESP.getPsramSize() / 1024);
    Serial.println("KB");
    Serial.print("Flash size: ");
    Serial.print(ESP.getFlashChipSize() / 1024);
    Serial.println("KB");
    Serial.print("CPU frequency: ");
    Serial.print(ESP.getCpuFreqMHz());
    Serial.println("MHz");
}

void led_on(int duration_ms) {
    digitalWrite(LED_PIN, HIGH);
    delay(duration_ms);
    digitalWrite(LED_PIN, LOW);
}

void led_blink_pattern(int blinks, int on_duration, int off_duration) {
    for(int i = 0; i < blinks; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(on_duration);
        digitalWrite(LED_PIN, LOW);
        if(i < blinks - 1) delay(off_duration);
    }
}

static lv_obj_t * status_label;
static lv_obj_t * counter_label;
static int button_press_count = 0;

static void short_blink_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        button_press_count++;
        lv_label_set_text_fmt(counter_label, "Button Pressed: %d times", button_press_count);
        lv_label_set_text(status_label, "LED On for 1s...");
        Serial.println("Short LED button clicked!");
        led_on(1000); // 1 second LED on
        lv_label_set_text(status_label, "Ready");
    }
}

static void long_blink_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        button_press_count++;
        lv_label_set_text_fmt(counter_label, "Button Pressed: %d times", button_press_count);
        lv_label_set_text(status_label, "LED On for 5s...");
        Serial.println("Long LED button clicked!");
        led_on(5000); // 5 second LED on
        lv_label_set_text(status_label, "Ready");
    }
}

static void pattern_blink_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        button_press_count++;
        lv_label_set_text_fmt(counter_label, "Button Pressed: %d times", button_press_count);
        lv_label_set_text(status_label, "LED Blinking...");
        Serial.println("Pattern LED button clicked!");
        led_blink_pattern(3, 200, 300); // 3 blinks, 200ms on, 300ms off
        lv_label_set_text(status_label, "Ready");
    }
}

static void reset_counter_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        button_press_count = 0;
        lv_label_set_text_fmt(counter_label, "Button Pressed: %d times", button_press_count);
        lv_label_set_text(status_label, "Counter Reset!");
        Serial.println("Counter reset!");
        delay(1000);
        lv_label_set_text(status_label, "Ready");
    }
}

void create_ui(void) {
    // Create a main container
    lv_obj_t * main_cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(main_cont, LV_PCT(95), LV_PCT(95));
    lv_obj_center(main_cont);
    
    // Set background color
    lv_obj_set_style_bg_color(main_cont, lv_color_hex(0x2E3440), LV_PART_MAIN);
    lv_obj_set_style_border_color(main_cont, lv_color_hex(0x5E81AC), LV_PART_MAIN);
    lv_obj_set_style_border_width(main_cont, 2, LV_PART_MAIN);
    
    // Title label
    lv_obj_t * title = lv_label_create(main_cont);
    lv_label_set_text(title, "T-HMI LED Controller");
    lv_obj_set_style_text_color(title, lv_color_hex(0xECEFF4), LV_PART_MAIN);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Status label
    status_label = lv_label_create(main_cont);
    lv_label_set_text(status_label, "Ready");
    lv_obj_set_style_text_color(status_label, lv_color_hex(0xA3BE8C), LV_PART_MAIN);
    lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 35);
    
    // Counter label
    counter_label = lv_label_create(main_cont);
    lv_label_set_text_fmt(counter_label, "Button Pressed: %d times", button_press_count);
    lv_obj_set_style_text_color(counter_label, lv_color_hex(0xD8DEE9), LV_PART_MAIN);
    lv_obj_align(counter_label, LV_ALIGN_TOP_MID, 0, 55);
    
    // Short LED button
    lv_obj_t * btn1 = lv_btn_create(main_cont);
    lv_obj_set_size(btn1, 180, 40);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0x5E81AC), LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0x81A1C1), LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn1, short_blink_handler, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * label1 = lv_label_create(btn1);
    lv_label_set_text(label1, "Short Blink (1s)");
    lv_obj_set_style_text_color(label1, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(label1);
    
    // Long LED button
    lv_obj_t * btn2 = lv_btn_create(main_cont);
    lv_obj_set_size(btn2, 180, 40);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0xBF616A), LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0xD08770), LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn2, long_blink_handler, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * label2 = lv_label_create(btn2);
    lv_label_set_text(label2, "Long Blink (5s)");
    lv_obj_set_style_text_color(label2, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(label2);
    
    // Pattern LED button
    lv_obj_t * btn3 = lv_btn_create(main_cont);
    lv_obj_set_size(btn3, 180, 40);
    lv_obj_align(btn3, LV_ALIGN_CENTER, 0, 60);
    lv_obj_set_style_bg_color(btn3, lv_color_hex(0xEBCB8B), LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn3, lv_color_hex(0xD08770), LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn3, pattern_blink_handler, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * label3 = lv_label_create(btn3);
    lv_label_set_text(label3, "Blink Pattern");
    lv_obj_set_style_text_color(label3, lv_color_black(), LV_PART_MAIN);
    lv_obj_center(label3);
    
    // Reset counter button
    lv_obj_t * btn4 = lv_btn_create(main_cont);
    lv_obj_set_size(btn4, 120, 30);
    lv_obj_align(btn4, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_bg_color(btn4, lv_color_hex(0x4C566A), LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn4, lv_color_hex(0x5E81AC), LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn4, reset_counter_handler, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * label4 = lv_label_create(btn4);
    lv_label_set_text(label4, "Reset Counter");
    lv_obj_set_style_text_color(label4, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(label4);
}


static void example_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
    lv_disp_flush_ready(drv);
}


static void lv_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    uint16_t x, y;
    if (touch.pressed()) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch.X();
        data->point.y = touch.Y();
        // touch_pin_get_int = false;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}


void setup() {
    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions
    static lv_indev_drv_t indev_drv;

    Serial.begin(115200);

#if USE_CALIBRATION_DATA
    data_init();
    data_read(calibration_data);
#endif

    SPI.begin(TOUCHSCREEN_SCLK_PIN, TOUCHSCREEN_MISO_PIN, TOUCHSCREEN_MOSI_PIN);
    touch.begin(240, 320);
#if USE_CALIBRATION_DATA
    touch.setCal(calibration_data[0].rawX, calibration_data[2].rawX, calibration_data[0].rawY, calibration_data[2].rawX, 240, 320); // Raw xmin, xmax, ymin, ymax, width, height
#else
    touch.setCal(1788, 285, 1877, 311, 240, 320); // Raw xmin, xmax, ymin, ymax, width, height
    Serial.println("Use default calibration data");
#endif
    touch.setRotation(0);
    print_chip_info();
    
    // Initialize LED pin
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    pinMode(PWR_EN_PIN, OUTPUT);
    digitalWrite(PWR_EN_PIN, HIGH);
    pinMode(EXAMPLE_PIN_NUM_BK_LIGHT, OUTPUT);
    digitalWrite(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);
    delay(200);

    Serial.println("Initialize Intel 8080 bus");
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = EXAMPLE_PIN_NUM_DC,
        .wr_gpio_num = EXAMPLE_PIN_NUM_PCLK,
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
        },
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    Serial.println("Install LCD driver of st7789");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // Send all the commands
    for (int i = 0; i < sizeof(st_init_cmds) / sizeof(lcd_init_cmd_t); i ++)
    {
        esp_lcd_panel_io_tx_param(io_handle, st_init_cmds[i].cmd, st_init_cmds[i].data, st_init_cmds[i].len);
        if (st_init_cmds[i].cmd & 0x11) {
            delay(st_init_cmds[i].delay);
        }
    }

    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES, gImage_logo);
    delay(3000);

    lv_init();
    // alloc draw buffers used by LVGL from PSRAM
    lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf1);
    lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf2);
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES);

    Serial.println("Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = example_lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lv_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    create_ui();
}


void loop() {
    delay(2);
    lv_timer_handler();
}
