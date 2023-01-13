#ifndef MONITOR_CONTROLLER_H_
#define MONITOR_CONTROLLER_H_

#include "MonitorModel.h"
#include "MonitorView.h"

class MonitorController
{
public:
    MonitorController();
    ~MonitorController();

    void init();
    void onViewLoad();
    void onViewDisappear();

private:
    MonitorModel model;
    MonitorView view;
    lv_timer_t *updateTimer;

private:
    void update();
    void deleteTimer();
    // void attachTemperatureMeterEvent();
    // void attachHumidityMeterEvent();
    // void onTemperatureMeterEvent();
    // void onHumidityMeterEvent();
};




#endif