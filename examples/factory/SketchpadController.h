#ifndef SKETCHPAD_CONTROLLER_H_
#define SKETCHPAD_CONTROLLER_H_

#include "SketchpadModel.h"
#include "SketchpadView.h"

class SketchpadController {
public:
    SketchpadController();
    ~SketchpadController();

    void init();
    void onViewLoad();
    void onViewDisappear();

private:
    SketchpadModel model;
    SketchpadView view;
    void attachToolbarEvent();
    void detachToolbarEvent();
    void attachCanvasEvent();
    void detachCanvasEvent();

private:
    static void onBrushLabelEvent(lv_event_t * e);
    static void onBrushWidthSliderEvent(lv_event_t * e);

    static void onBrushColorLabelEvent(lv_event_t * e);
    static void onBrushColorwheelEvent(lv_event_t * e);
    static void onRefreshLabelEvent(lv_event_t * e);

    static void onCanvasEvent(lv_event_t * e);
};


#endif