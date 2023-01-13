#include "TimerView.h"

#include "font/font_alibaba.h"

LV_FONT_DECLARE(font_Alibaba);

#define UI_BG_COLOR    lv_color_black()
#define UI_FRAME_COLOR lv_color_hex(0x282828)
#define UI_FONT_COLOR  lv_color_white()
#define UI_PAGE_COUNT  3

static lv_point_t line_points[] = {{-320, 0}, {320, 0}};

TimerView::TimerView() {
}

TimerView::~TimerView() {
}

void TimerView::create() {
    ui.root = lv_obj_create(NULL);

    ui.displayTileview = lv_tileview_create(ui.root);
    lv_obj_align(ui.displayTileview, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_size(ui.displayTileview, LV_PCT(100), LV_PCT(100));
    ui.clockTileview = lv_tileview_add_tile(ui.displayTileview, 0, 0, LV_DIR_ALL);

    ui.clockDiv = lv_obj_create(ui.clockTileview);
    lv_obj_set_size(ui.clockDiv, LV_PCT(100), LV_PCT(100));
    lv_obj_clear_flag(ui.clockDiv, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(ui.clockDiv, 0, 0);
    lv_obj_set_style_bg_color(ui.clockDiv, UI_BG_COLOR, 0);

    ui.hourDiv = lv_obj_create(ui.clockDiv);
    lv_obj_set_size(ui.hourDiv, 140, 140);
    lv_obj_align(ui.hourDiv, LV_ALIGN_CENTER, -85, 0);
    lv_obj_set_style_bg_color(ui.hourDiv, UI_FRAME_COLOR, 0);
    lv_obj_clear_flag(ui.hourDiv, LV_OBJ_FLAG_SCROLLABLE);

    ui.minDiv = lv_obj_create(ui.clockDiv);
    lv_obj_set_size(ui.minDiv, 140, 140);
    lv_obj_align(ui.minDiv, LV_ALIGN_CENTER, 85, 0);
    lv_obj_set_style_bg_color(ui.minDiv, UI_FRAME_COLOR, 0);
    lv_obj_clear_flag(ui.minDiv, LV_OBJ_FLAG_SCROLLABLE);

    ui.segLabel = lv_label_create(ui.clockDiv);
    lv_obj_align(ui.segLabel, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_text_font(ui.segLabel, &font_Alibaba, 0);
    lv_label_set_text(ui.segLabel, ":");
    lv_obj_set_style_text_color(ui.segLabel, UI_FONT_COLOR, 0);

    ui.hourLabel = lv_label_create(ui.hourDiv);
    lv_obj_center(ui.hourLabel);
    lv_obj_set_style_text_font(ui.hourLabel, &font_Alibaba, 0);
    lv_label_set_text(ui.hourLabel, "12");
    lv_obj_set_style_text_color(ui.hourLabel, UI_FONT_COLOR, 0);

    ui.minLabel = lv_label_create(ui.minDiv);
    lv_obj_center(ui.minLabel);
    lv_obj_set_style_text_font(ui.minLabel, &font_Alibaba, 0);
    lv_label_set_text(ui.minLabel, "34");
    lv_obj_set_style_text_color(ui.minLabel, UI_FONT_COLOR, 0);

    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 4);
    lv_style_set_line_color(&style_line, UI_BG_COLOR);
    lv_style_set_line_rounded(&style_line, true);

    ui.stripeLine = lv_line_create(ui.clockDiv);
    lv_line_set_points(ui.stripeLine, line_points, 2);
    lv_obj_add_style(ui.stripeLine, &style_line, 0);
    lv_obj_center(ui.stripeLine);
}


void TimerView::load() {
    lv_disp_load_scr(ui.root);
}


void TimerView::destroy() {
    if (ui.root != NULL) {
        lv_obj_del(ui.root);
        ui.root = NULL;
    }
}


void TimerView::setSegLabel(bool hidden) {
    if (hidden) {
        lv_obj_add_flag(ui.segLabel, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(ui.segLabel, LV_OBJ_FLAG_HIDDEN);
    }
}


void TimerView::updateHour(int32_t hour) {
    lv_label_set_text_fmt(ui.hourLabel, "%02d", hour);
}


void TimerView::updateMin(int32_t min) {
    lv_label_set_text_fmt(ui.minLabel, "%02d", min);
}
