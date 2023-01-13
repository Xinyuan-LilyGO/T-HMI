#include "TimerController.h"

TimerController::TimerController() {
}


TimerController::~TimerController() {
}


void TimerController::init() {
    model.init();
    view.create();
}


void TimerController::onViewLoad() {
    // model.init();
    // view.create();
    view.load();
    updateTimer = lv_timer_create(
        [](lv_timer_t* timer) {
            TimerController* controller = (TimerController *)timer->user_data;
            controller->update();
        },
        10000,
        this
    );

    tickTimer = lv_timer_create(
        [](lv_timer_t* timer) {
            TimerController* controller = (TimerController *)timer->user_data;
            controller->updateSegLabel();
        },
        500,
        this
    );
}


void TimerController::onViewDisappear() {
    deleteTimer();
    // view.destroy();
    // model.deinit();
}


void TimerController::update() {
    view.updateHour(model.getHour());
    view.updateMin(model.getMin());
}


void TimerController::updateSegLabel() {
    static bool status = false;
    view.setSegLabel(status);
    status = !status;
}


void TimerController::deleteTimer() {
    if (updateTimer != NULL) {
        lv_timer_del(updateTimer);
        updateTimer = NULL;
    }

    if (tickTimer != NULL) {
        lv_timer_del(tickTimer);
        tickTimer = NULL;
    }
}