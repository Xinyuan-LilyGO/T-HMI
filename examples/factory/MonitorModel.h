#ifndef MONITOR_MODEL_H_
#define MONITOR_MODEL_H_

#include <stdint.h>

class MonitorModel {
public:
    MonitorModel();
    ~MonitorModel();

    void init() {};
    void deinit() {};

    float getTemperature();
    long getHumidity();

private:

};


#endif