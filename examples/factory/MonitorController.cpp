#include "MonitorController.h"

MonitorController::MonitorController() {
}


MonitorController::~MonitorController() {
}


void MonitorController::init() {
    model.init();
    view.create();
}

void MonitorController::onViewLoad() {
    // model.init();
    // view.create();
    view.load();
    updateTimer = lv_timer_create(
        [](lv_timer_t* timer) {
            MonitorController* controller = (MonitorController *)timer->user_data;
            controller->update();
        },
        1000,
        this
    );
}


void MonitorController::onViewDisappear() {
    deleteTimer();
    // view.destroy();
    // model.deinit();
}


void MonitorController::update() {
    view.updateTemperature(model.getTemperature());
    view.updateHumidity(model.getHumidity());
}


void MonitorController::deleteTimer() {
    if (updateTimer != NULL) {
        lv_timer_del(updateTimer);
        updateTimer = NULL;
    }
}