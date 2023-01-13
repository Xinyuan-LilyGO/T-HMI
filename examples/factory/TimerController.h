#ifndef TIMER_CONTROLLER_H_
#define TIMER_CONTROLLER_H_

#include "TimerModel.h"
#include "TimerView.h"

class TimerController {
public:
    TimerController(/* args */);
    ~TimerController();

    void init();
    void onViewLoad();
    void onViewDisappear();

private:
    TimerModel model;
    TimerView view;
    lv_timer_t *updateTimer;
    lv_timer_t *tickTimer;

private:
    void update();
    void updateSegLabel();
    void deleteTimer();
};

#endif