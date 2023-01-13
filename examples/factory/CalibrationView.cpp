#include "CalibrationView.h"

CalibrationView::CalibrationView() {

}


CalibrationView::~CalibrationView() {

}


void CalibrationView::create() {
    ui.root = lv_obj_create(NULL);
    lv_obj_clear_flag(ui.root, LV_OBJ_FLAG_SCROLLABLE);

    ui.buttonPoints[0] = lv_btn_create(ui.root);
    lv_obj_set_width(ui.buttonPoints[0], 20);
    lv_obj_set_height(ui.buttonPoints[0], 20);
    lv_obj_set_align(ui.buttonPoints[0], LV_ALIGN_TOP_RIGHT);
    lv_obj_add_flag(
        ui.buttonPoints[0],
        LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_SCROLL_ON_FOCUS
    );
    lv_obj_clear_flag(ui.buttonPoints[0], LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(
        ui.buttonPoints[0],
        90,
        LV_PART_MAIN | LV_STATE_DEFAULT
    );

    lv_obj_t *label = lv_label_create(ui.buttonPoints[0]);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, "+");

    ui.buttonPoints[1] = lv_btn_create(ui.root);
    lv_obj_set_width(ui.buttonPoints[1], 20);
    lv_obj_set_height(ui.buttonPoints[1], 20);
    lv_obj_set_align(ui.buttonPoints[1], LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_add_flag(
        ui.buttonPoints[1],
        LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_SCROLL_ON_FOCUS
    );
    lv_obj_clear_flag(ui.buttonPoints[1], LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(
        ui.buttonPoints[1],
        90,
        LV_PART_MAIN | LV_STATE_DEFAULT
    );

    label = lv_label_create(ui.buttonPoints[1]);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, "+");

    ui.buttonPoints[2] = lv_btn_create(ui.root);
    lv_obj_set_width(ui.buttonPoints[2], 20);
    lv_obj_set_height(ui.buttonPoints[2], 20);
    lv_obj_set_align(ui.buttonPoints[2], LV_ALIGN_BOTTOM_LEFT);
    lv_obj_add_flag(
        ui.buttonPoints[2],
        LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_SCROLL_ON_FOCUS
    );
    lv_obj_clear_flag(ui.buttonPoints[2], LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(
        ui.buttonPoints[2],
        90,
        LV_PART_MAIN | LV_STATE_DEFAULT
    );

    label = lv_label_create(ui.buttonPoints[2]);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, "+");

    ui.buttonPoints[3] = lv_btn_create(ui.root);
    lv_obj_set_width(ui.buttonPoints[3], 20);
    lv_obj_set_height(ui.buttonPoints[3], 20);
    lv_obj_add_flag(
        ui.buttonPoints[3],
        LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_SCROLL_ON_FOCUS
    );
    lv_obj_clear_flag(ui.buttonPoints[3], LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(
        ui.buttonPoints[3],
        90,
        LV_PART_MAIN | LV_STATE_DEFAULT
    );

    label = lv_label_create(ui.buttonPoints[3]);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, "+");

    ui.labelTip = lv_label_create(ui.root);
    lv_obj_set_width(ui.labelTip, LV_SIZE_CONTENT);
    lv_obj_set_height(ui.labelTip, LV_SIZE_CONTENT);
    lv_obj_set_x(ui.labelTip, 1);
    lv_obj_set_y(ui.labelTip, -9);
    lv_obj_set_align(ui.labelTip, LV_ALIGN_CENTER);
    lv_label_set_text(ui.labelTip, "Click anywhere to start calibration");

    ui.buttonNext = lv_btn_create(ui.root);
    lv_obj_set_width(ui.buttonNext, 100);
    lv_obj_set_height(ui.buttonNext, 50);
    lv_obj_set_align(ui.buttonNext, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_add_flag(
        ui.buttonNext,
        LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_SCROLL_ON_FOCUS
    );
    lv_obj_clear_flag(ui.buttonNext, LV_OBJ_FLAG_SCROLLABLE);

    label = lv_label_create(ui.buttonNext);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, "Next");
}


void CalibrationView::load() {
    lv_disp_load_scr(ui.root);
}


void CalibrationView::destroy() {
    if (ui.root != NULL) {
        lv_obj_del(ui.root);
        ui.root = NULL;
    }
}


void CalibrationView::setPointx(lv_obj_t *buttonPoint, bool hidden) {
    if (buttonPoint == NULL) {
        return ;
    }

    if (hidden) {
        lv_obj_add_flag(buttonPoint, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(buttonPoint, LV_OBJ_FLAG_HIDDEN);
    }
}


void CalibrationView::setTipText(const char *text) {
    lv_label_set_text(ui.labelTip, text);
}
