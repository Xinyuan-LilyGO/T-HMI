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
inline const lv_color_t COL_BG = lv_color_hex(0x050807);
inline const lv_color_t COL_HEADER = lv_color_hex(0x080C09);
inline const lv_color_t COL_CARD = lv_color_hex(0x111813);
inline const lv_color_t COL_CARD_PRESSED = lv_color_hex(0x17341F);
inline const lv_color_t COL_BORDER = lv_color_hex(0x26332A);
inline const lv_color_t COL_ACCENT = lv_color_hex(0x4ADE4A);
inline const lv_color_t COL_TEXT = lv_color_hex(0xF2F7F2);
inline const lv_color_t COL_MUTED = lv_color_hex(0x8FA394);

} // namespace theme
