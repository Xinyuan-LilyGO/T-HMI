#ifndef CALIBRATION_MODEL_H_
#define CALIBRATION_MODEL_H_


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stdint.h>
#include <vector>

class TouchCalibration {
public:
    TouchCalibration() {};
    TouchCalibration(uint16_t x, uint16_t y): rawX(x),rawY(y){};
    uint16_t rawX;
    uint16_t rawY;

    void operator= (const TouchCalibration &rval) {
        rawX = rval.rawX;
        rawY = rval.rawY;
    }
};


class CalibrationModel {
public:
    CalibrationModel();
    ~CalibrationModel();

    void init();
    void deinit();
    bool readFromNVS();
    bool writeToNVS();
    bool getStatus();
    void set(TouchCalibration data, int32_t index);
    TouchCalibration get(int32_t index);

private:
    bool _calibrated = false;
    TouchCalibration _rawData[4];
};


#endif

