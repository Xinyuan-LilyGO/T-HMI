#ifndef CALIBRATION_CONTROLLER_H_
#define CALIBRATION_CONTROLLER_H_


#include "CalibrationModel.h"
#include "CalibrationView.h"

#include <xpt2046.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class CalibrationController {
public:
    XPT2046 *touch;
    CalibrationController(XPT2046 &touch);
    ~CalibrationController();

    void init();
    void initTask();
    void deinitTask();
    void onViewLoad();
    void onViewDisappear() ;
    TouchCalibration getCalibrationData(int32_t index);
    bool getCalibrationStatus();

private:
    CalibrationModel model;
    CalibrationView view;
    lv_indev_t oldIndev;
    lv_indev_drv_t indevDrv;
    void attachScreenEvent();
    void detachScreenEvent();
    void attachButtonEvent();
    void detachButtonEvent();
    void attachIndevDriver();
    void detachIndevDriver();

private:
    TaskHandle_t _calibrationTaskHandler;
    static void onScreenEvent(lv_event_t * e);
    static void onButton1Event(lv_event_t * e);
    static void onButton2Event(lv_event_t * e);
    static void onButton3Event(lv_event_t * e);
    static void onButton4Event(lv_event_t * e);
    static void calibrationTask(void *param);
    static void lv_touchpad_read(
        lv_indev_drv_t *indev_driver,
        lv_indev_data_t *data
    );
    bool _calibrationTaskState;
};


#endif
