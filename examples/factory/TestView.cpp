#include "TestView.h"


TestView::TestView() {

}


TestView::~TestView() {

}

void TestView::create() {
    ui.root = lv_obj_create(NULL);
    lv_obj_set_flex_flow(ui.root, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_clear_flag(ui.root, LV_OBJ_FLAG_SCROLLABLE);

    /** title */
    // ui.backButton = lv_obj_create(ui.root);
    // // lv_obj_set_size(ui.backButton, LV_PCT(15), LV_SIZE_CONTENT);
    // lv_obj_set_height(ui.backButton, LV_SIZE_CONTENT);
    // ui.backButtonLabel = lv_label_create(ui.backButton);
    // lv_obj_set_style_text_font(ui.backButtonLabel, &lv_font_montserrat_16, 0);
    // lv_obj_set_style_text_align(ui.backButtonLabel, LV_ALIGN_CENTER, 0);
    // lv_obj_set_align(ui.backButtonLabel, LV_ALIGN_CENTER);
    // lv_label_set_text(ui.backButtonLabel, "<");

    ui.titleDiv = lv_obj_create(ui.root);
    // lv_obj_add_style(ui.titleDiv, &style_title_div, 0);
    // lv_obj_set_size(ui.titleDiv, LV_PCT(70), LV_SIZE_CONTENT);
    lv_obj_set_height(ui.titleDiv, LV_SIZE_CONTENT);
    lv_obj_set_align(ui.titleDiv, LV_ALIGN_CENTER);

    ui.titleLabel = lv_label_create(ui.titleDiv);
    lv_obj_set_style_text_font(ui.titleDiv, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(ui.titleLabel, LV_ALIGN_CENTER, 0);
    lv_obj_set_align(ui.titleLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui.titleLabel, "Test");

    // ui.nextButton = lv_obj_create(ui.root);
    // // lv_obj_set_size(ui.nextButton, LV_PCT(15), LV_SIZE_CONTENT);
    // lv_obj_set_height(ui.nextButton, LV_SIZE_CONTENT);
    // ui.nextButtonLabel = lv_label_create(ui.nextButton);
    // lv_obj_set_style_text_font(ui.nextButtonLabel, &lv_font_montserrat_16, 0);
    // lv_obj_set_style_text_align(ui.nextButtonLabel, LV_ALIGN_CENTER, 0);
    // lv_obj_set_align(ui.nextButtonLabel, LV_ALIGN_CENTER);
    // lv_label_set_text(ui.nextButtonLabel, ">");

    // lv_obj_set_flex_grow(ui.backButton,1);
    lv_obj_set_flex_grow(ui.titleDiv,5);
    // lv_obj_set_flex_grow(ui.nextButton,1 );

    /** table */
    ui.resultDiv = lv_obj_create(ui.root);
    lv_obj_set_flex_flow(ui.resultDiv, LV_FLEX_FLOW_COLUMN_WRAP);
    lv_obj_clear_flag(ui.resultDiv, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(ui.resultDiv, 0, 0);
    lv_obj_set_size(ui.resultDiv, lv_obj_get_width(ui.root), LV_SIZE_CONTENT);
    lv_obj_set_flex_align(
        ui.resultDiv,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );

    ui.resultTable = lv_table_create(ui.resultDiv);
    lv_obj_set_flex_flow(ui.resultTable, LV_FLEX_FLOW_COLUMN_WRAP);
    lv_obj_set_style_text_font(
        ui.resultTable,
        &lv_font_montserrat_14,
        LV_PART_MAIN | LV_STATE_DEFAULT
    );
    lv_obj_set_style_pad_all(ui.resultTable, 1, LV_PART_MAIN);
    lv_obj_set_style_pad_all(ui.resultTable, 5, LV_PART_ITEMS);

    /** Fill the first column */
    lv_table_set_cell_value(ui.resultTable, 0, 0, "Function");
    lv_table_set_cell_value(ui.resultTable, 1, 0, "PSRAM");
    lv_table_set_cell_value(ui.resultTable, 2, 0, "Flash");
    lv_table_set_cell_value(ui.resultTable, 3, 0, "SD");
    lv_table_set_cell_value(ui.resultTable, 4, 0, "RSSI");
    lv_table_set_cell_value(ui.resultTable, 5, 0, "WIFI");

    /** Fill the second column */
    lv_table_set_cell_value(ui.resultTable, 0, 1, "Result");
    lv_table_set_cell_value(ui.resultTable, 1, 1, "Testing");
    lv_table_set_cell_value(ui.resultTable, 2, 1, "Testing");
    lv_table_set_cell_value(ui.resultTable, 3, 1, "Testing");
    lv_table_set_cell_value(ui.resultTable, 4, 1, "Testing");
    lv_table_set_cell_value(ui.resultTable, 5, 1, "Testing");

    /** Set a smaller height to the table. It'll make it scrollable */
    lv_table_set_col_cnt(ui.resultTable, 2);
    lv_table_set_row_cnt (ui.resultTable, 6);
    lv_table_set_col_width(ui.resultTable, 0, 120);
    lv_table_set_col_width(ui.resultTable, 1, lv_obj_get_width(ui.root) - 120 - 20);
    lv_obj_set_align(ui.resultTable, LV_ALIGN_TOP_LEFT);

    lv_obj_add_event_cb(ui.resultTable, drawPartEventCallback, LV_EVENT_DRAW_PART_BEGIN, NULL);
}


void TestView::load() {
    lv_disp_load_scr(ui.root);
}


void TestView::destroy() {
    if (ui.root != NULL) {
        lv_obj_del(ui.root);
        ui.root = NULL;
    }
}


void TestView::updateRamResult(const char *text) {
    if (text == NULL) {
        lv_table_set_cell_value(ui.resultTable, 1, 1, "Testing");
    } else {
        lv_table_set_cell_value(ui.resultTable, 1, 1, text);
    }
}


void TestView::updateFlashResult(const char *text) {
    if (text == NULL) {
        lv_table_set_cell_value(ui.resultTable, 2, 1, "Testing");
    } else {
        lv_table_set_cell_value(ui.resultTable, 2, 1, text);
    }
}


void TestView::updateSDResult(const char *text) {
    if (text == NULL) {
        lv_table_set_cell_value(ui.resultTable, 3, 1, "Testing");
    } else {
        lv_table_set_cell_value(ui.resultTable, 3, 1, text);
    }
}


void TestView::updateWiFiRSSIResult(const char *text) {
    if (text == NULL) {
        lv_table_set_cell_value(ui.resultTable, 4, 1, "Testing");
    } else {
        lv_table_set_cell_value(ui.resultTable, 4, 1, text);
    }
}


void TestView::updateWiFiConnectResult(const char *text) {
    if (text == NULL) {
        lv_table_set_cell_value(ui.resultTable, 5, 1, "Testing");
    } else {
        lv_table_set_cell_value(ui.resultTable, 5, 1, text);
    }
}


void TestView::drawPartEventCallback(lv_event_t * e) {
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);

    /** If the cells are drawn... */
    if (dsc->part == LV_PART_ITEMS) {
        uint32_t row = dsc->id / lv_table_get_col_cnt(obj);
        uint32_t col = dsc->id - row * lv_table_get_col_cnt(obj);

        /** Make the texts in the first cell center aligned */
        if (row == 0) {
            dsc->label_dsc->align = LV_TEXT_ALIGN_LEFT;
            dsc->rect_dsc->bg_color = lv_color_mix(
                lv_palette_main(LV_PALETTE_BLUE),
                dsc->rect_dsc->bg_color,
                LV_OPA_20
            );
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }

        /** MAke every 2nd row grayish */
        if ((row != 0 && row % 2) == 0) {
            dsc->rect_dsc->bg_color = lv_color_mix(
                lv_palette_main(LV_PALETTE_GREY),
                dsc->rect_dsc->bg_color,
                LV_OPA_10
            );
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }

        if (col == 1 && row != 0) {
            const char * txt = lv_table_get_cell_value(obj, row, col);
            if (!strncmp(txt, "FAIL", strlen("FAIL"))) {
                dsc->rect_dsc->bg_color = lv_palette_main(LV_PALETTE_RED);
            } else if (!strncmp(txt, "PASS", strlen("PASS"))) {
                dsc->rect_dsc->bg_color = lv_palette_main(LV_PALETTE_LIGHT_GREEN);
            }
            if (!strcmp(txt, "Testing")) {
                dsc->rect_dsc->bg_color = lv_palette_main(LV_PALETTE_RED);
            }
        }
    }
}