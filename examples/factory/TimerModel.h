#ifndef TIMER_MODEL_H_
#define TIMER_MODEL_H_

#include <stdint.h>

class TimerModel {
public:
    TimerModel(/* args */);
    ~TimerModel();

    void init();
    void deinit() {};
    int32_t getHour();
    int32_t getMin();

private:

};




#endif