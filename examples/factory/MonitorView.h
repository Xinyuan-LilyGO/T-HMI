#ifndef MONITOR_VIEW_H_
#define MONITOR_VIEW_H_

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

class MonitorView {
public:
    struct {
        /**
         * root
         *   -> titleDiv
         *     -> titleLabel
         *   -> temperatureMeterDiv
         *     -> temperatureMeter
         *     -> temperatureLable
         *   -> humidityMeterDiv
         *     -> humidityMeter
         *     -> humidityLable
         */
        lv_obj_t *root;
        lv_obj_t *titleDiv;
        lv_obj_t *titleLabel;
        lv_obj_t *temperatureMeterDiv;
        lv_obj_t *temperatureMeter;
        lv_obj_t *temperatureLable;
        lv_obj_t *humidityMeterDiv;
        lv_obj_t *humidityMeter;
        lv_obj_t *humidityLable;
    } ui;

    MonitorView();
    ~MonitorView();

    void create();
    void load();
    void destroy();

    void updateTemperature(float temperature);
    void updateHumidity(long humidity);
private:

};




#endif