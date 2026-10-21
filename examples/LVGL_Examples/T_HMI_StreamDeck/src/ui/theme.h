/**
 * @file theme.h
 * Dark theme, styles and the button factory for the deck UI.
 */
#pragma once

#include <lvgl.h>

namespace theme {

inline constexpr lv_coord_t HEADER_BUTTON_SIZE = 36;

void init(); // apply theme + create shared styles
lv_obj_t *make_button(lv_obj_t *parent, const char *icon, const char *label);
lv_obj_t *make_header_button(lv_obj_t *parent, const char *symbol);

// Shared styles (created once in init())
extern lv_style_t style_card; // button card look
extern lv_style_t style_card_pressed;
extern lv_style_t style_label;
extern lv_style_t style_icon;
extern lv_style_t style_header_button;
extern lv_style_t style_header_button_pressed;

// Palette
inline const lv_color_t COL_BG = lv_color_hex(0x101418);
inline const lv_color_t COL_CARD = lv_color_hex(0x1C232B);
inline const lv_color_t COL_ACCENT = lv_color_hex(0x00B4D8);
inline const lv_color_t COL_ACCENT_DARK = lv_color_hex(0x007A94);
inline const lv_color_t COL_TEXT = lv_color_hex(0xE8F1F2);

} // namespace theme
