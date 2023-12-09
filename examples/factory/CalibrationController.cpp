#include "CalibrationController.h"
#include <Arduino.h>


CalibrationController::CalibrationController(XPT2046 &touch) {
    this->touch = &touch;
}


CalibrationController::~CalibrationController() {

}



void CalibrationController::init() {
    model.init();
    view.create();
}


TouchCalibration CalibrationController::getCalibrationData(int32_t index) {
    return model.get(index);
}


bool CalibrationController::getCalibrationStatus() {
    return model.getStatus();
}


void CalibrationController::initTask() {
    xTaskCreatePinnedToCore(
        calibrationTask,
        "calibrationTask",
        4096,
        this,
        6,
        &_calibrationTaskHandler,
        1
    );
}


void CalibrationController::deinitTask() {
    if (_calibrationTaskState == false) {
        vTaskDelete(_calibrationTaskHandler);
    }
}


void CalibrationController::attachIndevDriver() {
    Serial.println(F("Registering input device to lvgl"));
    lv_indev_drv_init(&indevDrv);
    indevDrv.type = LV_INDEV_TYPE_POINTER;
    indevDrv.read_cb = lv_touchpad_read;
    indevDrv.user_data = touch;
    if (lv_indev_drv_register(&indevDrv) == NULL) {
        Serial.println(F("Failed to register input device to lvgl"));
    } else {
        Serial.println(F("Successfully registered input device to lvgl"));
    }
}


void CalibrationController::detachIndevDriver() {
    lv_indev_t *indev = lv_indev_get_next(NULL);
    if (indev != NULL) {
        memcpy(&oldIndev, indev, sizeof(lv_indev_t));
        lv_indev_delete(indev);
    } else {
        Serial.println("no input device");
    }
}


void CalibrationController::onViewLoad() {
    detachIndevDriver();
    initTask();
    // model.init();
    // view.create();
    view.setPointx(view.ui.buttonPoints[0], true);
    view.setPointx(view.ui.buttonPoints[1], true);
    view.setPointx(view.ui.buttonPoints[2], true);
    view.setPointx(view.ui.buttonPoints[3], true);
    view.setTipText("Click anywhere to start calibration");
    view.load();
    attachScreenEvent();
    attachButtonEvent();
}


void CalibrationController::onViewDisappear() {
    // view.destroy();
    // model.deinit();
    detachButtonEvent();
    deinitTask();
    attachIndevDriver();
}


void CalibrationController::attachScreenEvent() {
    lv_obj_add_event_cb(view.ui.root, onScreenEvent, LV_EVENT_ALL, this);
}


void CalibrationController::detachScreenEvent() {
    lv_obj_remove_event_cb(view.ui.root, onScreenEvent);
}


void CalibrationController::onScreenEvent(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    CalibrationController *controller = (CalibrationController *)lv_event_get_user_data(e);

    if(event_code == LV_EVENT_CLICKED) {
        controller->view.setPointx(controller->view.ui.buttonPoints[0], false);
        controller->view.setPointx(controller->view.ui.buttonPoints[1], true);
        controller->view.setTipText("Click the button in the upper right corner");
        controller->detachScreenEvent();
    }
}


void CalibrationController::attachButtonEvent() {
    lv_obj_add_event_cb(view.ui.buttonPoints[0], onButton1Event, LV_EVENT_ALL, this);
    lv_obj_add_event_cb(view.ui.buttonPoints[1], onButton2Event, LV_EVENT_ALL, this);
    lv_obj_add_event_cb(view.ui.buttonPoints[2], onButton3Event, LV_EVENT_ALL, this);
    lv_obj_add_event_cb(view.ui.buttonPoints[3], onButton4Event, LV_EVENT_ALL, this);
}


void CalibrationController::detachButtonEvent() {
    lv_obj_remove_event_cb(view.ui.buttonPoints[0], onButton1Event);
    lv_obj_remove_event_cb(view.ui.buttonPoints[0], onButton2Event);
    lv_obj_remove_event_cb(view.ui.buttonPoints[0], onButton3Event);
    lv_obj_remove_event_cb(view.ui.buttonPoints[0], onButton4Event);
}

void CalibrationController::onButton1Event(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    CalibrationController *controller = (CalibrationController *)lv_event_get_user_data(e);

    if(event_code == LV_EVENT_CLICKED) {
        controller->view.setPointx(controller->view.ui.buttonPoints[1], false);
        controller->view.setPointx(controller->view.ui.buttonPoints[0], true);
        controller->view.setTipText("Click the button in the lower right corner");
    }
}


void CalibrationController::onButton2Event(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    CalibrationController *controller = (CalibrationController *)lv_event_get_user_data(e);

    if(event_code == LV_EVENT_CLICKED) {
        controller->view.setPointx(controller->view.ui.buttonPoints[2], false);
        controller->view.setPointx(controller->view.ui.buttonPoints[1], true);
        controller->view.setTipText("Click the button in the lower left corner");
    }
}


void CalibrationController::onButton3Event(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    CalibrationController *controller = (CalibrationController*)lv_event_get_user_data(e);

    if(event_code == LV_EVENT_CLICKED) {
        controller->view.setPointx(controller->view.ui.buttonPoints[3], false);
        controller->view.setPointx(controller->view.ui.buttonPoints[2], true);
        controller->view.setTipText("Click the button in the upper left corner");
    }
}


void CalibrationController::onButton4Event(lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    CalibrationController *controller = (CalibrationController *)lv_event_get_user_data(e);

    if(event_code == LV_EVENT_CLICKED) {
        // controller->view.setPointx(controller->view.ui.buttonNext, false);
        controller->view.setPointx(controller->view.ui.buttonPoints[3], true);
        controller->view.setTipText("Calibration complete");
    }
}


void CalibrationController::calibrationTask(void *param) {
    CalibrationController *controller = (CalibrationController *)param;
    CalibrationModel model = controller->model;
    XPT2046 *touch = controller->touch;
    bool touchState = false;
    uint32_t lastTime = 0;
    uint8_t point = 0;
    std::vector<TouchCalibration> rawData;

    controller->_calibrationTaskState = false;
    while (1) {
        if (touch->pressed()) {
            touchState = true;
            lastTime = millis();
        }
        if (touchState && millis() - lastTime > 500) {
            touchState = false;
            lv_event_send(controller->view.ui.root, LV_EVENT_CLICKED, NULL);
            break ;
        }
        delay(10);
    }

    while (1) {
        if (touch->pressed()) {
            touchState = true;
            lastTime = millis();
            Serial.print("RAW X: ");
            Serial.print(touch->RawX());
            Serial.print(" RAW Y: ");
            Serial.println(touch->RawY());
            Serial.print("X: ");
            Serial.print(touch->X());
            Serial.print(" Y: ");
            Serial.println(touch->Y());

            rawData.push_back(TouchCalibration(touch->RawX(), touch->RawY()));
        }

        if (touchState && millis() - lastTime > 500) {
            touchState = false;
            uint32_t tmpX = 0;
            uint32_t tmpY = 0;

            for (size_t i = 0; i < rawData.size(); i++) {
                tmpX += rawData[i].rawX;
                tmpY += rawData[i].rawY;
            }

            model.set(
                TouchCalibration(
                    (tmpX / rawData.size()),
                    (tmpY / rawData.size())
                ),
                point
            );
            rawData.clear();

            lv_event_send(controller->view.ui.buttonPoints[point], LV_EVENT_CLICKED, NULL);
            if (point == 3) {
                model.writeToNVS();
                delay(100);
                touch->setCal(
                    model.get(0).rawX, // Raw xmin
                    model.get(2).rawX, // Raw xmax
                    model.get(0).rawY, // Raw ymin
                    model.get(2).rawY, // Raw ymax
                    240,               // width
                    320                // height
                );
                touch->setRotation(3);
                break ;
            }
            point++;
        }
        delay(10);
    }
    controller->_calibrationTaskState = true;
    vTaskDelete(NULL);
}


void CalibrationController::lv_touchpad_read(
    lv_indev_drv_t *indev_driver,
    lv_indev_data_t *data
) {
    XPT2046 *touch = (XPT2046 *)indev_driver->user_data;
    if (touch->pressed()) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch->X();
        data->point.y = touch->Y();
        // Serial.print("X: ");
        // Serial.print(data->point.x);
        // Serial.print(", Y: ");
        // Serial.println(data->point.y);
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}
