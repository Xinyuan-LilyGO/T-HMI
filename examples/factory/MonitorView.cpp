#include "MonitorView.h"


MonitorView::MonitorView() {
}


MonitorView::~MonitorView() {
}


void MonitorView::create() {
    /** screen */
    ui.root = lv_obj_create(NULL);
    // lv_obj_add_style(ui.root, &style_scr, 0);
    lv_obj_set_flex_flow(ui.root, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_clear_flag(ui.root, LV_OBJ_FLAG_SCROLLABLE);

    /** title */
    ui.titleDiv = lv_obj_create(ui.root);
    // lv_obj_add_style(ui.titleDiv, &style_title_div, 0);
    lv_obj_set_size(ui.titleDiv, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_align(ui.titleDiv, LV_ALIGN_CENTER);

    ui.titleLabel = lv_label_create(ui.titleDiv);
    lv_obj_set_style_text_font(ui.titleDiv, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(ui.titleLabel, LV_ALIGN_CENTER, 0);
    lv_obj_set_align(ui.titleLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui.titleLabel, "Monitor");

    /** temperature meter */
    ui.temperatureMeterDiv = lv_obj_create(ui.root);
    lv_obj_set_flex_flow(ui.temperatureMeterDiv, LV_FLEX_FLOW_COLUMN_WRAP);
    lv_obj_set_style_pad_all(ui.temperatureMeterDiv, 0, 0);
    lv_obj_set_size(ui.temperatureMeterDiv, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_align(
        ui.temperatureMeterDiv,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );

    ui.temperatureMeter = lv_meter_create(ui.temperatureMeterDiv);
    lv_obj_set_size(
        ui.temperatureMeter,
        (lv_obj_get_width(ui.root) - 2 * lv_obj_get_style_pad_column(ui.root, 0)) / 2,
        (lv_obj_get_width(ui.root) - 2 * lv_obj_get_style_pad_column(ui.root, 0)) / 2
    );

    /** Add a scale first */
    lv_meter_scale_t *scale;
    scale = lv_meter_add_scale(ui.temperatureMeter);
    lv_meter_set_scale_ticks(
        ui.temperatureMeter,
        scale,
        41,
        2,
        10,
        lv_palette_main(LV_PALETTE_GREY)
    );
    lv_meter_set_scale_major_ticks(
        ui.temperatureMeter,
        scale,
        8,
        4,
        15,
        lv_color_black(),
        10
    );

    /** Add a blue arc to the start */
    lv_meter_indicator_t *indic;
    indic = lv_meter_add_arc(
        ui.temperatureMeter,
        scale,
        3,
        lv_palette_main(LV_PALETTE_BLUE),
        0
    );
    lv_meter_set_indicator_start_value(ui.temperatureMeter, indic, 0);
    lv_meter_set_indicator_end_value(ui.temperatureMeter, indic, 20);

    /** Make the tick lines blue at the start of the scale */
    indic = lv_meter_add_scale_lines(
        ui.temperatureMeter,
        scale,
        lv_palette_main(LV_PALETTE_BLUE),
        lv_palette_main(LV_PALETTE_BLUE),
        false,
        0
    );
    lv_meter_set_indicator_start_value(ui.temperatureMeter, indic, 0);
    lv_meter_set_indicator_end_value(ui.temperatureMeter, indic, 20);

    /** Add a red arc to the end */
    indic = lv_meter_add_arc(
        ui.temperatureMeter,
        scale,
        3,
        lv_palette_main(LV_PALETTE_RED),
        0
    );
    lv_meter_set_indicator_start_value(ui.temperatureMeter, indic, 80);
    lv_meter_set_indicator_end_value(ui.temperatureMeter, indic, 100);

    /** Make the tick lines red at the end of the scale */
    indic = lv_meter_add_scale_lines(
        ui.temperatureMeter,
        scale,
        lv_palette_main(LV_PALETTE_RED),
        lv_palette_main(LV_PALETTE_RED),
        false,
        0
    );
    lv_meter_set_indicator_start_value(ui.temperatureMeter, indic, 80);
    lv_meter_set_indicator_end_value(ui.temperatureMeter, indic, 100);

    /** Add a needle line indicator */
    indic = lv_meter_add_needle_line(
        ui.temperatureMeter,
        scale,
        4,
        lv_palette_main(LV_PALETTE_GREY),
        -10
    );
    ui.temperatureMeter->user_data = indic;
    ui.temperatureLable = lv_label_create(ui.temperatureMeterDiv);
    lv_label_set_text(ui.temperatureLable, "22 °C");

    /** humidity meter */
    ui.humidityMeterDiv = lv_obj_create(ui.root);
    lv_obj_set_flex_flow(ui.humidityMeterDiv, LV_FLEX_FLOW_COLUMN_WRAP);
    lv_obj_set_style_pad_all(ui.humidityMeterDiv, 0, 0);
    lv_obj_set_size(ui.humidityMeterDiv, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_align(
        ui.humidityMeterDiv,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );

    ui.humidityMeter = lv_meter_create(ui.humidityMeterDiv);
    lv_obj_set_size(
        ui.humidityMeter,
        (lv_obj_get_width(ui.root) - 2 * lv_obj_get_style_pad_column(ui.root, 0)) / 2,
        (lv_obj_get_width(ui.root) - 2 * lv_obj_get_style_pad_column(ui.root, 0)) / 2
    );


    /** Add a scale first */
    lv_meter_scale_t *humidity_scale = lv_meter_add_scale(ui.humidityMeter);
    lv_meter_set_scale_ticks(
        ui.humidityMeter,
        humidity_scale,
        41,
        2,
        10,
        lv_palette_main(LV_PALETTE_GREY)
    );
    lv_meter_set_scale_major_ticks(
        ui.humidityMeter,
        humidity_scale,
        8,
        4,
        15,
        lv_color_black(),
        10
    );

    lv_meter_indicator_t * humidity_indic;
    /** Add a blue arc to the start */
    humidity_indic = lv_meter_add_arc(
        ui.humidityMeter,
        humidity_scale,
        3,
        lv_palette_main(LV_PALETTE_BLUE),
        0
    );
    lv_meter_set_indicator_start_value(ui.humidityMeter, humidity_indic, 0);
    lv_meter_set_indicator_end_value(ui.humidityMeter, humidity_indic, 20);

    /** Make the tick lines blue at the start of the scale */
    humidity_indic = lv_meter_add_scale_lines(
        ui.humidityMeter,
        humidity_scale,
        lv_palette_main(LV_PALETTE_BLUE),
        lv_palette_main(LV_PALETTE_BLUE),
        false,
        0
    );
    lv_meter_set_indicator_start_value(ui.humidityMeter, humidity_indic, 0);
    lv_meter_set_indicator_end_value(ui.humidityMeter, humidity_indic, 20);

    /** Add a red arc to the end */
    humidity_indic = lv_meter_add_arc(
        ui.humidityMeter,
        humidity_scale,
        3,
        lv_palette_main(LV_PALETTE_RED),
        0
    );
    lv_meter_set_indicator_start_value(ui.humidityMeter, humidity_indic, 80);
    lv_meter_set_indicator_end_value(ui.humidityMeter, humidity_indic, 100);

    /** Make the tick lines red at the end of the scale */
    humidity_indic = lv_meter_add_scale_lines(
        ui.humidityMeter,
        humidity_scale,
        lv_palette_main(LV_PALETTE_RED),
        lv_palette_main(LV_PALETTE_RED),
        false,
        0
    );
    lv_meter_set_indicator_start_value(ui.humidityMeter, humidity_indic, 80);
    lv_meter_set_indicator_end_value(ui.humidityMeter, humidity_indic, 100);

    /** Add a needle line indicator */
    humidity_indic = lv_meter_add_needle_line(
        ui.humidityMeter,
        humidity_scale,
        4,
        lv_palette_main(LV_PALETTE_GREY),
        -10
    );

    ui.humidityLable = lv_label_create(ui.humidityMeterDiv);
    lv_label_set_text(ui.humidityLable, "55%");
    lv_obj_center(ui.humidityLable);
    ui.humidityMeter->user_data = humidity_indic;
}


void MonitorView::load() {
    lv_disp_load_scr(ui.root);
}


void MonitorView::destroy() {
    if (ui.root != NULL) {
        lv_obj_del(ui.root);
        ui.root = NULL;
    }
}

#include <Arduino.h>
void MonitorView::updateTemperature(float temperature) {
    static long last_value = 0;
    // Serial.printf("temperature: %f\r\n", temperature);
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_exec_cb(
        &anim,
        (lv_anim_exec_xcb_t)[](void *meter, int32_t v) {
            lv_meter_set_indicator_value(
                (lv_obj_t *)meter,
                (lv_meter_indicator_t *)((lv_obj_t *)meter)->user_data,
                v
            );
        }
    );
    lv_anim_set_var(&anim, ui.temperatureMeter);
    lv_anim_set_values(&anim, last_value, (int32_t)temperature);
    lv_anim_set_time(&anim, 200);
    lv_anim_set_repeat_count(&anim, 0);
    lv_anim_start(&anim);

    lv_label_set_text_fmt(ui.temperatureLable, "%.2f °C", temperature);

    last_value = temperature;
}


void MonitorView::updateHumidity(long humidity) {
    static long last_value = 0;
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_exec_cb(
        &anim,
        (lv_anim_exec_xcb_t)[](void *meter, int32_t v) {
            lv_meter_set_indicator_value(
                (lv_obj_t *)meter,
                (lv_meter_indicator_t *)((lv_obj_t *)meter)->user_data,
                v
            );
        }
    );
    lv_anim_set_var(&anim, ui.humidityMeter);
    lv_anim_set_values(&anim, last_value, humidity);
    lv_anim_set_time(&anim, 200);
    lv_anim_set_repeat_count(&anim, 0);
    lv_anim_start(&anim);

    lv_label_set_text_fmt(ui.humidityLable, "%lu %%", humidity);

    last_value = humidity;
}

