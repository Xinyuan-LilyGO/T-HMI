#ifndef TIMER_VIEW_H_
#define TIMER_VIEW_H_

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

class TimerView {
public:

    struct {
        /**
         * root
         *   -> displayTileview
         *     -> clockTileview
         *       -> clockDiv
         *         -> hourDiv
         *           -> hourLabel
         *         -> minDiv
         *           -> minLabel
         *         -> segLabel
         *         -> Line
         */
        lv_obj_t *root;
        lv_obj_t *displayTileview;
        lv_obj_t *clockTileview;
        lv_obj_t *clockDiv;
        lv_obj_t *hourDiv;
        lv_obj_t *hourLabel;
        lv_obj_t *minDiv;
        lv_obj_t *minLabel;
        lv_obj_t *segLabel;
        lv_obj_t *stripeLine;
    } ui;

    TimerView();
    ~TimerView();

    void create();
    void load();
    void destroy();

    void setSegLabel(bool hidden);
    void updateHour(int32_t hour);
    void updateMin(int32_t min);
private:
};




#endif