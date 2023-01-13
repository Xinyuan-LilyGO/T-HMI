#ifndef SKETCHPAD_VIEW_H_
#define SKETCHPAD_VIEW_H_

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

class SketchpadView {
public:
    struct {
        /**
         * root
         *   -> toolbar
         *     -> color
         *     -> brush
         *   -> Canvas
         *   -> Colorwheel
        */
        lv_obj_t *root;
        lv_obj_t *toolbarDiv;
        lv_obj_t *brushColorLabel;
        lv_obj_t *brushLabel;
        lv_obj_t *refreshLabel;
        lv_obj_t *brushColorwheel;
        lv_obj_t *brushWidthSlider;
        lv_obj_t *canvas;
        lv_draw_line_dsc_t brush;
    } ui;

    SketchpadView(/* args */);
    ~SketchpadView();

    void create();
    void load();
    void destroy();

private:
};



#endif