/**
 * @file config.h
 * USER-EDITABLE: pages, buttons and the HID actions they send.
 *
 * Single source of truth for the whole deck. Add a page or remap a button
 * here; the UI is built from this table at boot.
 *
 * Layout: landscape 320x240, 3 columns x 2 rows, up to 6 buttons per page.
 * Bad page layouts (empty page, too many buttons, missing label/icon) fail the
 * build through static_assert. Actions are validated once at boot because the
 * Arduino GCC 8.4 toolchain cannot constant-evaluate the action table.
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include <lvgl.h>

#include "actions.h"

// ---------------------------------------------------------------------------
// Button / page model
// ---------------------------------------------------------------------------

struct ButtonDef {
    const char *label; // text under the icon
    const char *icon;  // LVGL symbol, e.g. LV_SYMBOL_PLAY
    ButtonAction action;
};

constexpr size_t MAX_BUTTONS_PER_PAGE = 6;

struct PageDef {
    const char *title;
    ButtonDef buttons[MAX_BUTTONS_PER_PAGE];
    size_t button_count;
};

template <size_t N>
constexpr PageDef make_page(const char *title, const ButtonDef (&buttons)[N]) {
    static_assert(N > 0, "config.h: a page needs at least one button");
    static_assert(N <= MAX_BUTTONS_PER_PAGE, "config.h: too many buttons for a page");
    PageDef page{title, {}, N};
    for (size_t i = 0; i < N; ++i) page.buttons[i] = buttons[i];
    return page;
}

// ---------------------------------------------------------------------------
// PAGE TABLE — edit freely. Keycodes are HID_KEY_* usage IDs from TinyUSB.
// ---------------------------------------------------------------------------

constexpr ButtonDef MEDIA_BUTTONS[] = {
    {"Play",  LV_SYMBOL_PLAY,  Media(CONSUMER_PLAY_PAUSE)},
    {"Next",  LV_SYMBOL_NEXT,  Media(CONSUMER_SCAN_NEXT)},
    {"Prev",  LV_SYMBOL_PREV,  Media(CONSUMER_SCAN_PREV)},
    {"Vol +", LV_SYMBOL_PLUS,  Media(CONSUMER_VOL_UP)},
    {"Vol -", LV_SYMBOL_MINUS, Media(CONSUMER_VOL_DOWN)},
    {"Mute",  LV_SYMBOL_MUTE,  Media(CONSUMER_MUTE)},
};

// F13-F18: bind them in OBS under Settings > Hotkeys to avoid conflicts.
constexpr ButtonDef OBS_BUTTONS[] = {
    {"Scene 1", LV_SYMBOL_VIDEO,   Keys(MOD_NONE, HID_KEY_F13)},
    {"Scene 2", LV_SYMBOL_VIDEO,   Keys(MOD_NONE, HID_KEY_F14)},
    {"Scene 3", LV_SYMBOL_VIDEO,   Keys(MOD_NONE, HID_KEY_F15)},
    {"Mic",     LV_SYMBOL_AUDIO,   Keys(MOD_NONE, HID_KEY_F16)},
    {"Stream",  LV_SYMBOL_PLAY,    Keys(MOD_NONE, HID_KEY_F17)},
    {"Replay",  LV_SYMBOL_REFRESH, Keys(MOD_NONE, HID_KEY_F18)},
};

// Default Teams/Zoom/Meet combos; remap to taste.
constexpr ButtonDef MEETING_BUTTONS[] = {
    {"Mic",   LV_SYMBOL_AUDIO,    Keys(MOD_CTRL | MOD_SHIFT, HID_KEY_M)},      // Ctrl+Shift+M
    {"Cam",   LV_SYMBOL_VIDEO,    Keys(MOD_CTRL | MOD_SHIFT, HID_KEY_O)},      // Ctrl+Shift+O
    {"Hand",  LV_SYMBOL_OK,       Keys(MOD_ALT, HID_KEY_Y)},                   // Alt+Y
    {"React", LV_SYMBOL_BELL,     Keys(MOD_CTRL | MOD_SHIFT, HID_KEY_P)},      // Ctrl+Shift+P
    {"Leave", LV_SYMBOL_CLOSE,    Keys(MOD_CTRL | MOD_SHIFT, HID_KEY_H)},      // Ctrl+Shift+H
    {"Emoji", LV_SYMBOL_SETTINGS, Keys(MOD_GUI, HID_KEY_PERIOD)},              // Win+.
};

constexpr ButtonDef SYSTEM_BUTTONS[] = {
    {"Snip",    LV_SYMBOL_IMAGE,     Keys(MOD_GUI | MOD_SHIFT, HID_KEY_S)},    // Win+Shift+S
    {"Copy",    LV_SYMBOL_COPY,      Keys(MOD_CTRL, HID_KEY_C)},               // Ctrl+C
    {"Paste",   LV_SYMBOL_PASTE,     Keys(MOD_CTRL, HID_KEY_V)},               // Ctrl+V
    {"Undo",    LV_SYMBOL_LEFT,      Keys(MOD_CTRL, HID_KEY_Z)},               // Ctrl+Z
    {"Desktop", LV_SYMBOL_HOME,      Keys(MOD_GUI, HID_KEY_D)},                // Win+D
    {"Lock",    LV_SYMBOL_EYE_CLOSE, Keys(MOD_GUI, HID_KEY_L)},                // Win+L
};

inline constexpr PageDef PAGES[] = {
    make_page("Media", MEDIA_BUTTONS),
    make_page("OBS", OBS_BUTTONS),
    make_page("Meetings", MEETING_BUTTONS),
    make_page("System", SYSTEM_BUTTONS),
};

inline constexpr size_t PAGE_COUNT = sizeof(PAGES) / sizeof(PAGES[0]);

// ---------------------------------------------------------------------------
// Compile-time validation
// ---------------------------------------------------------------------------

// Layout checks are evaluated at compile time. The GCC 8.4 toolchain shipped
// with the Arduino core cannot constant-evaluate reads of ButtonAction members
// through the page table, so action checks run once at boot instead.
constexpr bool layout_is_valid(const PageDef &page) {
    if (page.title == nullptr) return false;
    if (page.button_count == 0 || page.button_count > MAX_BUTTONS_PER_PAGE) return false;
    for (size_t i = 0; i < page.button_count; ++i) {
        if (page.buttons[i].label == nullptr || page.buttons[i].icon == nullptr) return false;
    }
    return true;
}

constexpr bool all_layouts_are_valid() {
    for (const PageDef &page : PAGES) {
        if (!layout_is_valid(page)) return false;
    }
    return true;
}

static_assert(all_layouts_are_valid(), "config.h: invalid page layout");

// Call once at boot; see setup() in main.cpp.
inline bool config_is_valid() {
    for (const PageDef &page : PAGES) {
        if (!layout_is_valid(page)) return false;
        for (size_t i = 0; i < page.button_count; ++i) {
            if (!action_is_valid(page.buttons[i].action)) return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// Behavior settings
// ---------------------------------------------------------------------------

constexpr uint32_t IDLE_DIM_TIMEOUT_MS = 30000; // dim the screen after idle
constexpr uint32_t SPLASH_DURATION_MS = 1500;

static_assert(IDLE_DIM_TIMEOUT_MS > 0, "config.h: idle timeout must be positive");
static_assert(SPLASH_DURATION_MS > 0, "config.h: splash duration must be positive");
