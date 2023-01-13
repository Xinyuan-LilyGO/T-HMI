#include "SketchpadController.h"

#include <Arduino.h>

SketchpadController::SketchpadController() {
}


SketchpadController::~SketchpadController() {
}


void SketchpadController::init() {
    view.create();
}


void SketchpadController::onViewLoad() {
    view.load();
    attachToolbarEvent();
    attachCanvasEvent();
}


void SketchpadController::onViewDisappear() {
    detachToolbarEvent();
    detachCanvasEvent();
}


void SketchpadController::attachToolbarEvent() {
    lv_obj_add_event_cb(view.ui.brushColorLabel, onBrushColorLabelEvent, LV_EVENT_ALL, this);
    lv_obj_add_event_cb(view.ui.brushLabel, onBrushLabelEvent, LV_EVENT_ALL, this);
    lv_obj_add_event_cb(view.ui.brushWidthSlider, onBrushWidthSliderEvent, LV_EVENT_ALL, this);
    lv_obj_add_event_cb(view.ui.brushColorwheel, onBrushColorwheelEvent, LV_EVENT_ALL, this);
    lv_obj_add_event_cb(view.ui.refreshLabel, onRefreshLabelEvent, LV_EVENT_ALL, this);
}


void SketchpadController::detachToolbarEvent() {
    lv_obj_remove_event_cb(view.ui.brushColorLabel, onBrushColorLabelEvent);
    lv_obj_remove_event_cb(view.ui.brushLabel, onBrushLabelEvent);
    lv_obj_remove_event_cb(view.ui.brushWidthSlider, onBrushWidthSliderEvent);
    lv_obj_remove_event_cb(view.ui.brushColorwheel, onBrushColorwheelEvent);
    lv_obj_remove_event_cb(view.ui.refreshLabel, onRefreshLabelEvent);
}


void SketchpadController::attachCanvasEvent() {
    lv_obj_add_event_cb(view.ui.canvas, onCanvasEvent, LV_EVENT_ALL, this);
}


void SketchpadController::detachCanvasEvent() {
    lv_obj_remove_event_cb(view.ui.canvas, onCanvasEvent);
}


void SketchpadController::onBrushColorLabelEvent(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    SketchpadController *controller = (SketchpadController *)lv_event_get_user_data(e);

    // Serial.print("onBrushColorLabelEvent Event Code: ");
    // Serial.println(code);

    if (code == LV_EVENT_CLICKED) {
        lv_obj_clear_flag(controller->view.ui.brushColorwheel, LV_OBJ_FLAG_HIDDEN);
        controller->detachCanvasEvent();
    }
}


void SketchpadController::onBrushLabelEvent(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    SketchpadController *controller = (SketchpadController *)lv_event_get_user_data(e);

    // Serial.print("onBrushLabelEvent Event Code: ");
    // Serial.println(code);

    if (code == LV_EVENT_CLICKED) {
        lv_obj_clear_flag(controller->view.ui.brushWidthSlider, LV_OBJ_FLAG_HIDDEN);
        controller->detachCanvasEvent();
    }
}


void SketchpadController::onBrushWidthSliderEvent(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *slider = lv_event_get_target(e);
    SketchpadController *controller = (SketchpadController *)lv_event_get_user_data(e);

    // Serial.print("onBrushWidthSliderEvent Event Code: ");
    // Serial.println(code);

    if (code == LV_EVENT_DEFOCUSED) {
        lv_obj_add_flag(controller->view.ui.brushWidthSlider, LV_OBJ_FLAG_HIDDEN);
        controller->attachCanvasEvent();
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        controller->view.ui.brush.width = (lv_coord_t)lv_slider_get_value(slider);
    }
}



void SketchpadController::onBrushColorwheelEvent(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *colorwheel = lv_event_get_target(e);
    SketchpadController *controller = (SketchpadController *)lv_event_get_user_data(e);

    // Serial.print("onBrushColorwheelEvent Event Code: ");
    // Serial.println(code);

    if (code == LV_EVENT_DEFOCUSED) {
        lv_obj_add_flag(controller->view.ui.brushColorwheel, LV_OBJ_FLAG_HIDDEN);
        controller->attachCanvasEvent();
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        controller->view.ui.brush.color = lv_colorwheel_get_rgb(colorwheel);
    }
}


void SketchpadController::onRefreshLabelEvent(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    SketchpadController *controller = (SketchpadController *)lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED) {
        lv_canvas_fill_bg(
            controller->view.ui.canvas,
            lv_palette_lighten(LV_PALETTE_GREY, 3),
            LV_OPA_COVER
        );
    }
}



void SketchpadController::onCanvasEvent(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    SketchpadController *controller = (SketchpadController *)lv_event_get_user_data(e);
    static lv_coord_t last_x, last_y = -32768;

    // Serial.print("onCanvasEvent Code: ");
    // Serial.println(code);

    if (code == LV_EVENT_PRESSING) {
        lv_indev_t * indev = lv_indev_get_act();
        if(indev == NULL) return;

        lv_point_t point;
        lv_indev_get_point(indev, &point);

        lv_color_t c0;
        c0.full = 10;

        lv_point_t points[2];

        /*Release or first use*/
        if ((last_x == -32768) || (last_y == -32768)) {
            last_x = point.x;
            last_y = point.y;
        } else {
            points[0].x = last_x;
            points[0].y = last_y;
            points[1].x = point.x;
            points[1].y = point.y;
            last_x = point.x;
            last_y = point.y;
            lv_canvas_draw_line(obj, points, 2, &controller->view.ui.brush);
        }
    } else if(code == LV_EVENT_RELEASED) {
        last_x = -32768;
        last_y = -32768;
    }

}