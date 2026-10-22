/**
 * @file theme.cpp
 * Dark theme + button factory implementation.
 */
#include "theme.h"

namespace theme {

lv_style_t style_card;
lv_style_t style_card_pressed;
lv_style_t style_label;
lv_style_t style_icon;
lv_style_t style_header_button;
lv_style_t style_header_button_pressed;

void init() {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, COL_BG, 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    // Card / button style
    lv_style_init(&style_card);
    lv_style_set_bg_color(&style_card, COL_CARD);
    lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
    lv_style_set_radius(&style_card, 14);
    lv_style_set_border_width(&style_card, 1);
    lv_style_set_border_color(&style_card, COL_BORDER);
    lv_style_set_shadow_width(&style_card, 0);
    lv_style_set_pad_all(&style_card, 8);

    // Subtle green wash on press keeps the accent visible without a shadow.
    lv_style_init(&style_card_pressed);
    lv_style_set_bg_color(&style_card_pressed, COL_CARD_PRESSED);
    lv_style_set_border_color(&style_card_pressed, COL_ACCENT);
    lv_style_set_border_width(&style_card_pressed, 1);

    // Label
    lv_style_init(&style_label);
    lv_style_set_text_color(&style_label, COL_TEXT);
    lv_style_set_text_font(&style_label, &lv_font_montserrat_16);

    // Icon
    lv_style_init(&style_icon);
    lv_style_set_text_color(&style_icon, COL_ACCENT);
    lv_style_set_text_font(&style_icon, &lv_font_montserrat_22);

    // Header page-arrow buttons
    lv_style_init(&style_header_button);
    lv_style_set_bg_opa(&style_header_button, LV_OPA_TRANSP);
    lv_style_set_border_width(&style_header_button, 1);
    lv_style_set_border_color(&style_header_button, COL_BORDER);
    lv_style_set_radius(&style_header_button, 10);
    lv_style_set_pad_all(&style_header_button, 0);
    lv_style_set_shadow_width(&style_header_button, 0);

    lv_style_init(&style_header_button_pressed);
    lv_style_set_bg_color(&style_header_button_pressed, COL_CARD_PRESSED);
    lv_style_set_bg_opa(&style_header_button_pressed, LV_OPA_COVER);
    lv_style_set_border_width(&style_header_button_pressed, 1);
    lv_style_set_border_color(&style_header_button_pressed, COL_ACCENT);
}

lv_obj_t *make_button(lv_obj_t *parent, const char *icon, const char *label) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_add_style(btn, &style_card, 0);
    lv_obj_add_style(btn, &style_card_pressed, LV_STATE_PRESSED);
    lv_obj_set_size(btn, LV_PCT(100), LV_PCT(100));
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(btn, 6, 0);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *ic = lv_label_create(btn);
    lv_obj_add_style(ic, &style_icon, 0);
    lv_label_set_text(ic, icon != nullptr ? icon : "");

    lv_obj_t *lb = lv_label_create(btn);
    lv_obj_add_style(lb, &style_label, 0);
    lv_label_set_long_mode(lb, LV_LABEL_LONG_CLIP);
    lv_obj_set_width(lb, LV_PCT(100));
    lv_obj_set_style_text_align(lb, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lb, label != nullptr ? label : "");

    return btn;
}

lv_obj_t *make_header_button(lv_obj_t *parent, const char *symbol) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_add_style(btn, &style_header_button, 0);
    lv_obj_add_style(btn, &style_header_button_pressed, LV_STATE_PRESSED);
    lv_obj_set_size(btn, HEADER_BUTTON_SIZE, HEADER_BUTTON_SIZE);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label = lv_label_create(btn);
    lv_obj_set_style_text_color(label, COL_ACCENT, 0);
    lv_label_set_text(label, symbol != nullptr ? symbol : "");
    lv_obj_center(label);

    return btn;
}

} // namespace theme
