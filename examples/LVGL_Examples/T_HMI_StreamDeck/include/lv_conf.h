/**
 * @file lv_conf.h
 * LVGL 8.3 configuration for LilyGo T-HMI Stream Deck.
 * Only essential settings; everything else uses LVGL defaults.
 */

#ifndef LV_CONF_H
#define LV_CONF_H

/* Color depth: RGB565 for the ST7789 panel */
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 0

/* Memory: use PSRAM via Arduino heap */
#define LV_MEM_CUSTOM 1
#define LV_MEM_CUSTOM_INCLUDE "Arduino.h"
#define LV_MEM_CUSTOM_ALLOC malloc
#define LV_MEM_CUSTOM_FREE free
#define LV_MEM_CUSTOM_REALLOC realloc

/* Use the standard lvgl tick via LV_TICK_CUSTOM=1 build flag (millis) */
#define LV_TICK_CUSTOM 1
#define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())

#define LV_DPI_DEF 130

/* Fonts: keep default Montserrat set */
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_22 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_DEFAULT &lv_font_montserrat_16

/* Symbols (icons) enabled by default */

/* Widgets */
#define LV_USE_TILEVIEW 0
#define LV_USE_BTNMATRIX 0
#define LV_USE_TABVIEW 0
#define LV_USE_KEYBOARD 0
#define LV_USE_MSGBOX 0
#define LV_USE_SPAN 0
#define LV_USE_TABLE 0
#define LV_USE_CALENDAR 0
#define LV_USE_CHART 0
#define LV_USE_SPINNER 0

/* Extra widgets: keep small */
#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/* Animations */
#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR 0

/* Logging */
#define LV_USE_LOG 0

#endif /* LV_CONF_H */
