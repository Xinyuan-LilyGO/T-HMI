#include "TestController.h"

TestController::TestController() {

}


TestController::~TestController() {

}


void TestController::init() {
    // model.init();
    view.create();
}

void TestController::onViewLoad() {
    model.init();
    // view.create();
    view.load();
    updateTimer = lv_timer_create(
        [](lv_timer_t* timer) {
            TestController* controller = (TestController *)timer->user_data;
            controller->update();
        },
        1000,
        this
    );
}


void TestController::onViewDisappear() {
    deleteTimer();
    model.deinit();
    // view.destroy();
    // model.deinit();
}


void TestController::update() {
    view.updateRamResult(model.getRamSizeTestResult().c_str());
    view.updateFlashResult(model.getFlashTestResult().c_str());
    view.updateSDResult(model.getSDTestResult().c_str());
    view.updateWiFiRSSIResult(model.getWiFiRSSIResult().c_str());
    view.updateWiFiConnectResult(model.getWiFiConnectResult().c_str());
}


void TestController::deleteTimer() {
    if (updateTimer != NULL) {
        lv_timer_del(updateTimer);
        updateTimer = NULL;
    }
}