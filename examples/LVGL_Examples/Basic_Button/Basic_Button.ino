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

#include <Wire.h>
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

void scan_iic(void) {
    byte error, address;
    int nDevices = 0;
    Serial.println("Scanning for I2C devices ...");
    for (address = 0x01; address < 0x7f; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
            Serial.printf("I2C device found at address 0x%02X\n", address);
            nDevices++;
        } else if (error != 2) {
            Serial.printf("Error %d at address 0x%02X\n", error, address);
        }
    }
    if (nDevices == 0) {
        Serial.println("No I2C devices found");
    }
}


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

  // Initialize Serial (debug output) and optionally load saved calibration
  Serial.begin(115200);

#if USE_CALIBRATION_DATA
  // Initialize NVS and attempt to read stored calibration data
  data_init();
  data_read(calibration_data);
#endif

  // Initialize SPI and the XPT2046 touch if present
  SPI.begin(TOUCHSCREEN_SCLK_PIN, TOUCHSCREEN_MISO_PIN, TOUCHSCREEN_MOSI_PIN);
  touch.begin(240, 320);

  // Apply calibration: either the persisted values or default constants
#if USE_CALIBRATION_DATA
  touch.setCal(calibration_data[0].rawX, calibration_data[2].rawX, calibration_data[0].rawY, calibration_data[2].rawX, 240, 320); // Raw xmin, xmax, ymin, ymax, width, height
#else
  touch.setCal(1788, 285, 1877, 311, 240, 320); // example defaults
  Serial.println("Use default calibration data");
#endif
  touch.setRotation(0);

  // Print chip info (helps verify PSRAM presence and memory size)
  print_chip_info();

  // Power-on sequence for the panel (enable power and backlight)
  pinMode(PWR_EN_PIN, OUTPUT);
  digitalWrite(PWR_EN_PIN, HIGH);
  pinMode(EXAMPLE_PIN_NUM_BK_LIGHT, OUTPUT);
  digitalWrite(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);
  delay(200);

  // Initialize the Intel 8080 (i80) parallel bus used by this panel
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

  // Send the initialization command sequence (st_init_cmds) to the panel
  // Each entry contains a command, optional parameters, and an optional post-delay.
  for (int i = 0; i < sizeof(st_init_cmds) / sizeof(lcd_init_cmd_t); i ++)
  {
    esp_lcd_panel_io_tx_param(io_handle, st_init_cmds[i].cmd, st_init_cmds[i].data, st_init_cmds[i].len);
    if (st_init_cmds[i].cmd & 0x11) {
      delay(st_init_cmds[i].delay);
    }
  }



  // Initialize LVGL and allocate two full-screen draw buffers from PSRAM.
  // This is fast for rendering but requires PSRAM. If allocation fails, the
  // assert() will halt execution — consider adding a graceful fallback.
  lv_init();
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

  // Register the touch input device with LVGL so widgets receive pointer events
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = lv_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  lv_create_main_gui();
}

void lv_create_main_gui(void) {
  /* Create and apply a simple style for a white background and default black text.
   * This keeps the rest of the UI readable when using a white background.
   */
  static lv_style_t style_screen;
  lv_style_init(&style_screen);
#if LV_VERSION_CHECK(8, 0, 0)
  lv_style_set_bg_color(&style_screen, lv_color_white());
  lv_style_set_bg_opa(&style_screen, LV_OPA_COVER);
#else
  lv_style_set_bg_color(&style_screen, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_bg_opa(&style_screen, LV_STATE_DEFAULT, LV_OPA_COVER);
#endif
  lv_obj_add_style(lv_scr_act(), &style_screen, 0);

  /* Create a button on the active screen
   * - lv_btn_create: create a button object parented to the active screen
   * - lv_obj_set_size: set the size in pixels
   * - lv_obj_center: center the button on the parent
   */
  lv_obj_t * button = lv_btn_create(lv_scr_act());
  lv_obj_set_size(button, 120, 50);
  lv_obj_center(button);

  /* Create a label inside the button for the button caption
   * The label is parented to the button so it moves with it.
   */
  lv_obj_t * button_label = lv_label_create(button);
  lv_label_set_text(button_label, "Click here!");        // Set the label's text
  lv_obj_center(button_label);
  /* Ensure button label text is black for visibility on white background */
#if LV_VERSION_CHECK(8, 0, 0)
  lv_obj_set_style_text_color(button_label, lv_color_black(), 0);
#else
  lv_obj_set_style_local_text_color(button_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
#endif

  /* Create a separate label on the screen to show a counter value
   * - We parent it to the screen (lv_scr_act()) so it is independent of the button
   * - lv_obj_align places it at the bottom middle with a small upward offset
   */
  lv_obj_t * text_label_counter = lv_label_create(lv_scr_act());
  lv_label_set_text(text_label_counter, "Counter: 0");
  lv_obj_align(text_label_counter, LV_ALIGN_BOTTOM_MID, 0, -50);
  /* Ensure counter label text is black for visibility on white background */
#if LV_VERSION_CHECK(8, 0, 0)
  lv_obj_set_style_text_color(text_label_counter, lv_color_black(), 0);
#else
  lv_obj_set_style_local_text_color(text_label_counter, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
#endif

  /* Register an event callback for the button
   * - We only register for LV_EVENT_CLICKED to avoid unnecessary calls for other event types
   * - The last parameter (user_data) is a pointer passed to the callback; here we pass
   *   the counter label so the callback can update it directly.
   */
  lv_obj_add_event_cb(button, button_event_cb, LV_EVENT_CLICKED, text_label_counter);  // Assign a callback to the button

}

static void button_event_cb(lv_event_t * e) {
  /* Event callback for the button
   * - e: LVGL event object containing the event code, target and user_data
   * - We retrieve the event code and the target object (the button). The target is
   *   currently unused, but kept for future use (e.g., to change styles on press).
   * - The user_data parameter is expected to be a pointer to the label that shows the counter.
   */
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * button = (lv_obj_t*) lv_event_get_target(e);
  (void)button; // intentionally unused at the moment
  lv_obj_t * label = (lv_obj_t*) lv_event_get_user_data(e);

  /* Only respond to click events. Using LV_EVENT_CLICKED reduces overhead compared
   * to handling all event types.
   */
  if (code == LV_EVENT_CLICKED) {
    /* Keep a static counter that increments on each click.
     * - uint32_t chosen to reduce quick wrapping when clicking many times
     * - Cast to unsigned long when printing to match printf expectations across platforms
     */
    static uint32_t counter = 0;
    counter++;

    /* Update the label if the user_data pointer is valid. If it's NULL, log a message
     * to Serial so debugging is easier on platforms where LVGL logging may not be enabled.
     */
    if (label) {
      lv_label_set_text_fmt(label, "Counter: %lu", (unsigned long)counter);
    } else {
      Serial.printf("button_event_cb: label user_data is NULL\n");
    }

    /* Also print the counter to Serial for visibility during development
     * (useful when the display or label isn't available).
     */
    Serial.printf("Counter: %lu\n", (unsigned long)counter);
  }
}

void loop() {
  lv_timer_handler();  // let the GUI do its work
  delay(2);           // let this time pass
}

