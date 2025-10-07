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

// Arc object used by the gauge (declared globally so set_temp can access it)
static lv_obj_t * arc = NULL;

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
  // Seed the random number generator so demo values vary between boots
  randomSeed(micros());
}

// Create the main GUI: a white background, a blue arc-style gauge centered on the screen,
// and a dark, high-contrast label in the middle showing the temperature value.
// The gauge indicator is intentionally thicker for better visibility on small displays.
void lv_create_main_gui(void) {
  // Make the screen background white for a clean look
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);

  // Create an Arc
  arc = lv_arc_create(lv_scr_act());
  lv_obj_set_size(arc, 210, 210);
  lv_arc_set_rotation(arc, 135);
  lv_arc_set_bg_angles(arc, 0, 270);
  // Make the indicator blue and thicker for better visibility on white
  lv_obj_set_style_arc_color(arc, lv_color_hex(0x1E90FF), LV_PART_INDICATOR); // DodgerBlue
  lv_obj_set_style_arc_width(arc, 12, LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(arc, lv_color_hex(0xFFFFFF), LV_PART_KNOB);
  lv_obj_align(arc, LV_ALIGN_CENTER, 0, 10);

  // Create a text label in font size 32 to display the latest temperature reading
  lv_obj_t * text_label_temp_value = lv_label_create(lv_scr_act());
  lv_label_set_text(text_label_temp_value, "--.--");
  lv_obj_set_style_text_align(text_label_temp_value, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(text_label_temp_value, LV_ALIGN_CENTER, 0, 10);
  static lv_style_t style_temp;
  lv_style_init(&style_temp);
  // Text styling: larger, dark color and slightly increased spacing for better legibility.
  // NOTE: `lv_font_montserrat_24` isn't provided in this project build by default,
  // revert to the compiled-in font to avoid linker errors. To use a true 24px
  // Montserrat font you must add the font C file to the project or enable it
  // properly in your LVGL font build.
  lv_style_set_text_font(&style_temp, &lv_font_montserrat_14);
  lv_style_set_text_color(&style_temp, lv_color_hex(0x111111));
  lv_style_set_text_opa(&style_temp, LV_OPA_COVER);
  lv_style_set_text_letter_space(&style_temp, 1);
  lv_obj_add_style(text_label_temp_value, &style_temp, 0);

  // Create a timer to update the text label with the latest temperature value every 10 seconds
  lv_timer_create(timer_cb, 10000, text_label_temp_value);
}

// Timer callback function
static void timer_cb(lv_timer_t * timer) {
  // Retrieve user data label
  lv_obj_t * label_temp = (lv_obj_t *)timer->user_data;
  // Call the set_temp function
  set_temp(label_temp);
}

// Set the temperature value in the arc and text label
// This function is currently in demo mode: it generates a random temperature value
// (using TEMP_ARC_MIN/TEMP_ARC_MAX if available, otherwise falling back to
// sensible Celsius/Fahrenheit ranges), maps that value to the arc (0-100) and
// updates the centered label. Replace with real sensor reads if you enable the
// BME sensor and remove the demo behavior.
static void set_temp(void * text_label_temp_value) {
  // Generate a demo/random temperature value instead of reading the real sensor.
  // Use TEMP_ARC_MIN/TEMP_ARC_MAX when available, otherwise fall back to sensible ranges.
  float bme280_temp = 0.0;
  float min_temp = 0.0;
  float max_temp = 0.0;

  #if TEMP_CELSIUS
    // Celsius defaults
    #ifdef TEMP_ARC_MIN
      min_temp = (float)TEMP_ARC_MIN;
    #else
      min_temp = -20.0;
    #endif
    #ifdef TEMP_ARC_MAX
      max_temp = (float)TEMP_ARC_MAX;
    #else
      max_temp = 50.0;
    #endif
  #else
    // Fahrenheit defaults
    #ifdef TEMP_ARC_MIN
      min_temp = (float)TEMP_ARC_MIN;
    #else
      min_temp = -4.0;    // ~ -20 C
    #endif
    #ifdef TEMP_ARC_MAX
      max_temp = (float)TEMP_ARC_MAX;
    #else
      max_temp = 122.0;   // ~ 50 C
    #endif
  #endif

  // Safety: if min/max are inverted or equal, reset to sane defaults
  if (min_temp >= max_temp) {
    #if TEMP_CELSIUS
      min_temp = -20.0; max_temp = 50.0;
    #else
      min_temp = -4.0; max_temp = 122.0;
    #endif
  }

  // Generate a random value with two decimals precision
  long scaled_min = (long)round(min_temp * 100.0);
  long scaled_max = (long)round(max_temp * 100.0);
  long r = random(scaled_min, scaled_max + 1);
  bme280_temp = ((float)r) / 100.0;

  // Keep the label text color constant (dark) for better visibility on white background.
  // A true bold font would be ideal; simulate stronger text by letter spacing and full opacity.
  const char degree_symbol[] =
  #if TEMP_CELSIUS
    "\u00B0C";
  #else
    "\u00B0F";
  #endif

  // Map temperature into 0-100 for the arc. Use integer mapping; fall back to computed min/max when macros absent.
  int temp_for_map = (int)round(bme280_temp);
  int arc_min = (int)round(min_temp);
  int arc_max = (int)round(max_temp);
  int arc_value = map(temp_for_map, arc_min, arc_max, 0, 100);
  arc_value = constrain(arc_value, 0, 100);
  lv_arc_set_value(arc, arc_value);

  // Show with two decimal places
  String bme280_temp_text = String(bme280_temp, 2) + degree_symbol;
  lv_label_set_text((lv_obj_t*) text_label_temp_value, bme280_temp_text.c_str());
  Serial.print("Temperature: ");
  Serial.println(bme280_temp_text);
}

void loop() {
  lv_timer_handler();  // let the GUI do its work
  delay(2);           // let this time pass
}

