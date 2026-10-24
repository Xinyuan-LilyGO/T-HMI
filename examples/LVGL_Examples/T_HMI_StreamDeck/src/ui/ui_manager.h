/**
 * @file ui_manager.h
 * Builds the stacked page views, header navigation, and HID button wiring.
 */
#pragma once

#include <lvgl.h>

#include <cstdint>

namespace ui {

void build();                       // create the whole UI
void show_splash(uint32_t duration_ms); // brand splash before the deck UI
void notify_activity();             // call on touch to reset idle dim timer

} // namespace ui
