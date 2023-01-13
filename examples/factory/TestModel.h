#ifndef TEST_MODEL_H_
#define TEST_MODEL_H_

#include <WString.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class TestModel {
public:
    TestModel();
    ~TestModel();

    void init();
    void deinit();

    String getRamSizeTestResult();
    String getFlashTestResult();
    String getSDTestResult();
    String getWiFiRSSIResult();
    String getWiFiConnectResult();

private:
    TaskHandle_t _testTaskHandler;
    String _sdTestResult;

private:
    static void testTask(void *param);
};


#endif
