#ifndef TEST_VIEW_H_
#define TEST_VIEW_H_


#include "lvgl.h"

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
