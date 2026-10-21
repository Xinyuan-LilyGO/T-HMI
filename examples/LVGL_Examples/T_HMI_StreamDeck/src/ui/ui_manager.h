/**
 * @file ui_manager.h
 * Builds stacked page views (one per PageDef), header page tabs and
 * previous/next buttons, and wires button clicks to HID actions.
 */
#pragma once

#include <lvgl.h>

#include <cstdint>

namespace ui {

void build();                       // create the whole UI
void show_splash(uint32_t duration_ms); // brand splash before the deck UI
void notify_activity();             // call on touch to reset idle dim timer

} // namespace ui
