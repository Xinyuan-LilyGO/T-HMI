#include "SketchpadView.h"

#include <esp_heap_caps.h>

SketchpadView::SketchpadView() {
}


SketchpadView::~SketchpadView() {
}


void SketchpadView::create() {
    ui.root = lv_obj_create(NULL);

    ui.toolbarDiv = lv_obj_create(ui.root);
    lv_obj_align(ui.toolbarDiv, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_flex_grow(ui.toolbarDiv, 1);
    lv_obj_set_flex_flow(ui.toolbarDiv, LV_FLEX_FLOW_ROW);
    lv_obj_set_size(ui.toolbarDiv, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_flag(ui.toolbarDiv, LV_OBJ_FLAG_CLICKABLE);

    ui.brushColorLabel = lv_label_create(ui.toolbarDiv);
    lv_label_set_text(ui.brushColorLabel, LV_SYMBOL_EDIT);
    lv_obj_add_flag(ui.brushColorLabel, LV_OBJ_FLAG_CLICKABLE);

    ui.brushLabel = lv_label_create(ui.toolbarDiv);
    lv_label_set_text(ui.brushLabel, LV_SYMBOL_EJECT);
    lv_obj_add_flag(ui.brushLabel, LV_OBJ_FLAG_CLICKABLE);

    ui.refreshLabel = lv_label_create(ui.toolbarDiv);
    lv_label_set_text(ui.refreshLabel, LV_SYMBOL_REFRESH);
    lv_obj_add_flag(ui.refreshLabel, LV_OBJ_FLAG_CLICKABLE);

    ui.brushColorwheel = lv_colorwheel_create(ui.root, true);
    lv_obj_set_size(ui.brushColorwheel, 150, 150);
    lv_obj_align_to(ui.brushColorwheel, ui.brushColorLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
    lv_obj_add_flag(ui.brushColorwheel, LV_OBJ_FLAG_HIDDEN);

    ui.brushWidthSlider = lv_slider_create(ui.root);
    lv_slider_set_value(ui.brushWidthSlider, 10, LV_ANIM_OFF);
    // lv_obj_align_to(ui.brushWidthSlider, ui.brushLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);
    lv_obj_center(ui.brushWidthSlider);
    lv_obj_add_flag(ui.brushWidthSlider, LV_OBJ_FLAG_HIDDEN);
    lv_slider_set_range(ui.brushWidthSlider, 2, 20);

    ui.canvas = lv_canvas_create(ui.root);
    lv_color_t *buf = (lv_color_t *)heap_caps_malloc(
        320 * 240 * sizeof(lv_color_t),
        MALLOC_CAP_SPIRAM
    );
    lv_canvas_set_buffer(ui.canvas, buf, 320, 240, LV_IMG_CF_TRUE_COLOR);
    lv_canvas_fill_bg(
        ui.canvas,
        lv_palette_lighten(LV_PALETTE_GREY, 3),
        LV_OPA_COVER
    );
    lv_obj_move_background(ui.canvas);
    lv_obj_add_flag(ui.canvas, LV_OBJ_FLAG_CLICKABLE);

    lv_draw_line_dsc_init(&ui.brush);
    ui.brush.width = 2;
    ui.brush.round_start = true;
    ui.brush.round_end = true;
    ui.brush.color = lv_color_black();
    ui.brush.opa = LV_OPA_COVER;

}


void SketchpadView::load() {
    lv_disp_load_scr(ui.root);
}


void SketchpadView::destroy() {
    if (ui.root != NULL) {
        lv_obj_del(ui.root);
        ui.root = NULL;
    }
}
