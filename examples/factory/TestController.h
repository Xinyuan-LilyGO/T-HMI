#ifndef TEST_CONTROLLER_H_
#define TEST_CONTROLLER_H_

#include "TestModel.h"
#include "TestView.h"

class TestController
{
public:
    TestController(/* args */);
    ~TestController();

    void init();
    void onViewLoad();
    void onViewDisappear();

private:
    TestModel model;
    TestView view;
    lv_timer_t *updateTimer;

private:
    void update();
    void deleteTimer();
};



#endif
