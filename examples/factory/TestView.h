#ifndef TEST_VIEW_H_
#define TEST_VIEW_H_

#if defined __has_include
#if __has_include("lvgl.h")
#include "lvgl.h"
#elif __has_include("lvgl/lvgl.h")
#include "lvgl/lvgl.h"
#else
#include "lvgl.h"
#endif
#else
#include "lvgl.h"
#endif

class TestView {
public:
    struct {
        /**
         * root
         *   -> backButton
         *     -> backButtonLabel
         *   -> titleDiv
         *     -> titleLabel
         *   -> nextButton
         *     -> nextButtonLabel
         *   -> result_div
         *     -> result_table
         */
        lv_obj_t *root;
        lv_obj_t *backButton;
        lv_obj_t *backButtonLabel;
        lv_obj_t *titleDiv;
        lv_obj_t *titleLabel;
        lv_obj_t *nextButton;
        lv_obj_t *nextButtonLabel;
        lv_obj_t *resultDiv;
        lv_obj_t *resultTable;
    } ui;

    TestView();
    ~TestView();

    void create();
    void load();
    void destroy();

    void updateRamResult(const char *text);
    void updateFlashResult(const char *text);
    void updateSDResult(const char *text);
    void updateWiFiRSSIResult(const char *text);
    void updateWiFiConnectResult(const char *text);

private:
    static void drawPartEventCallback(lv_event_t * e);
};




#endif
