#include "MonitorModel.h"
#include <Arduino.h>

MonitorModel::MonitorModel(/* args */)
{

}


MonitorModel::~MonitorModel()
{

}


float MonitorModel::getTemperature() {
    return temperatureRead();
}


long MonitorModel::getHumidity() {
    return random(50, 100);
}
