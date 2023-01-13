#ifndef CALIBRATION_VIEW_H_
#define CALIBRATION_VIEW_H_

#if defined __has_include
#if __has_include("lvgl.h")
#include "lvgl.h"
#elif __has_include("lvgl/lvgl.h")
#include "lvgl/lvgl.h"
#else
#include "lvgl.h"
#endif
#else
#include "lvgl.h"
#endif

class CalibrationView {
public:
    struct {
        lv_obj_t *root;
        lv_obj_t *labelTip;
        lv_obj_t *buttonPoints[4];
        lv_obj_t *buttonNext;
    } ui;

    CalibrationView();
    ~CalibrationView();

    void create();
    void load();
    void destroy();

    void setPointx(lv_obj_t *buttonPoint, bool hidden);
    void setTipText(const char *text);
private:
};



#endif