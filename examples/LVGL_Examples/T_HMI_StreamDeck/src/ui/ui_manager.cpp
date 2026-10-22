#include "ui_manager.h"

#include <Arduino.h>

#include <cstddef>
#include <cstdint>

#include "backlight.h"
#include "config.h"
#include "theme.h"
#include "../hid_manager.h"

namespace ui {

namespace {

constexpr lv_coord_t HEADER_HEIGHT = 48;
constexpr lv_coord_t GRID_GAP = 10;
constexpr size_t GRID_COLUMNS_COUNT = 3;
constexpr size_t GRID_ROWS_COUNT = 2;
constexpr lv_coord_t GRID_COLUMNS[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
constexpr lv_coord_t GRID_ROWS[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

static_assert(PAGE_COUNT > 0, "config.h must define at least one page");
static_assert(PAGE_COUNT <= 8, "Too many pages for the header page tabs");
static_assert(MAX_BUTTONS_PER_PAGE <= GRID_COLUMNS_COUNT * GRID_ROWS_COUNT,
              "Grid cannot hold all configured buttons");

lv_obj_t *title_label;
lv_obj_t *page_views[PAGE_COUNT] = {};
size_t current_page = 0;
uint32_t last_activity = 0;
bool dimmed = false;

void set_dimmed(bool value) {
    if (dimmed == value) return;
    dimmed = value;
    backlight::set_dimmed(value);
}

void button_event(lv_event_t *event) {
    if (lv_event_get_code(event) != LV_EVENT_PRESSED) return;
    auto *definition = static_cast<const ButtonDef *>(lv_event_get_user_data(event));
    if (definition == nullptr) return;
    notify_activity();
    HID.send(definition->action);
}

void update_page_indicator(size_t index) {
    if (index >= PAGE_COUNT) return;
    lv_label_set_text(title_label, PAGES[index].title);
}

void show_page(size_t index) {
    if (index >= PAGE_COUNT || page_views[index] == nullptr) return;

    for (size_t i = 0; i < PAGE_COUNT; ++i) {
        if (page_views[i] == nullptr) continue;
        if (i == index) {
            lv_obj_clear_flag(page_views[i], LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(page_views[i], LV_OBJ_FLAG_HIDDEN);
        }
    }

    current_page = index;
    update_page_indicator(index);
    lv_obj_update_layout(page_views[index]);
    lv_obj_invalidate(page_views[index]);
    notify_activity();
}

void step_page(int delta) {
    const int count = static_cast<int>(PAGE_COUNT);
    if (count <= 1) return;
    const int next = ((static_cast<int>(current_page) + delta) % count + count) % count;
    show_page(static_cast<size_t>(next));
}

void previous_page_event(lv_event_t *event) {
    if (lv_event_get_code(event) == LV_EVENT_PRESSED) step_page(-1);
}

void next_page_event(lv_event_t *event) {
    if (lv_event_get_code(event) == LV_EVENT_PRESSED) step_page(1);
}

void idle_timer(lv_timer_t *) {
    if (static_cast<uint32_t>(lv_tick_get() - last_activity) >= IDLE_DIM_TIMEOUT_MS) set_dimmed(true);
}

void first_refresh_timer(lv_timer_t *timer) {
    lv_obj_invalidate(lv_scr_act());
    lv_timer_del(timer);
}

} // namespace

void show_splash(uint32_t duration_ms) {
    lv_obj_t *deck = lv_scr_act();
    lv_obj_t *splash = lv_obj_create(nullptr);
    lv_obj_remove_style_all(splash);
    lv_obj_set_style_bg_color(splash, theme::COL_BG, 0);
    lv_obj_set_style_bg_opa(splash, LV_OPA_COVER, 0);

    lv_obj_t *brand = lv_label_create(splash);
    lv_label_set_text(brand, "FUSION AUTOMATE");
    lv_obj_set_style_text_color(brand, theme::COL_ACCENT, 0);
    lv_obj_set_style_text_font(brand, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_letter_space(brand, 2, 0);
    lv_obj_align(brand, LV_ALIGN_CENTER, 0, -18);

    lv_obj_t *line = lv_obj_create(splash);
    lv_obj_remove_style_all(line);
    lv_obj_set_size(line, 64, 3);
    lv_obj_set_style_bg_color(line, theme::COL_ACCENT, 0);
    lv_obj_set_style_bg_opa(line, LV_OPA_COVER, 0);
    lv_obj_align(line, LV_ALIGN_CENTER, 0, 8);

    lv_obj_t *tagline = lv_label_create(splash);
    lv_label_set_text(tagline, "LEARN. BUILD. AUTOMATE.");
    lv_obj_set_style_text_color(tagline, theme::COL_MUTED, 0);
    lv_obj_set_style_text_font(tagline, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_letter_space(tagline, 1, 0);
    lv_obj_align(tagline, LV_ALIGN_CENTER, 0, 30);

    lv_scr_load(splash);
    lv_timer_handler();
    delay(300);
    backlight::set_dimmed(false);

    const uint32_t start = lv_tick_get();
    while (static_cast<uint32_t>(lv_tick_get() - start) < duration_ms) {
        lv_timer_handler();
        delay(5);
    }

    lv_scr_load(deck);
    lv_obj_del(splash);
    notify_activity(); // start the idle timeout after the splash, not before it
}

void notify_activity() {
    last_activity = lv_tick_get();
    set_dimmed(false);
}

void build() {
    lv_obj_t *screen = lv_scr_act();
    theme::init();
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *header = lv_obj_create(screen);
    lv_obj_remove_style_all(header);
    lv_obj_set_size(header, LV_PCT(100), HEADER_HEIGHT);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, theme::COL_HEADER, 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_border_side(header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(header, 1, 0);
    lv_obj_set_style_border_color(header, theme::COL_BORDER, 0);
    lv_obj_update_layout(header);

    title_label = lv_label_create(header);
    lv_obj_add_style(title_label, &theme::style_label, 0);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_letter_space(title_label, 1, 0);
    lv_obj_set_width(title_label, 120);
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *prev_button = theme::make_header_button(header, LV_SYMBOL_LEFT);
    lv_obj_align(prev_button, LV_ALIGN_LEFT_MID, 12, 0);
    lv_obj_set_ext_click_area(prev_button, 24);
    lv_obj_add_event_cb(prev_button, previous_page_event, LV_EVENT_PRESSED, nullptr);

    lv_obj_t *next_button = theme::make_header_button(header, LV_SYMBOL_RIGHT);
    lv_obj_align(next_button, LV_ALIGN_RIGHT_MID, -12, 0);
    lv_obj_set_ext_click_area(next_button, 24);
    lv_obj_add_event_cb(next_button, next_page_event, LV_EVENT_PRESSED, nullptr);

    lv_obj_t *content = lv_obj_create(screen);
    lv_obj_remove_style_all(content);
    lv_obj_set_size(content, LV_PCT(100), LV_VER_RES - HEADER_HEIGHT);
    lv_obj_set_pos(content, 0, HEADER_HEIGHT);
    lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);

    for (size_t page_index = 0; page_index < PAGE_COUNT; ++page_index) {
        lv_obj_t *page_view = lv_obj_create(content);
        lv_obj_remove_style_all(page_view);
        lv_obj_set_size(page_view, LV_PCT(100), LV_PCT(100));
        lv_obj_set_pos(page_view, 0, 0);
        lv_obj_set_style_pad_all(page_view, GRID_GAP, 0);
        lv_obj_add_flag(page_view, LV_OBJ_FLAG_HIDDEN);
        page_views[page_index] = page_view;

        lv_obj_t *grid = lv_obj_create(page_view);
        lv_obj_remove_style_all(grid);
        lv_obj_set_size(grid, LV_PCT(100), LV_PCT(100));
        lv_obj_set_layout(grid, LV_LAYOUT_GRID);
        lv_obj_set_grid_dsc_array(grid, GRID_COLUMNS, GRID_ROWS);
        lv_obj_set_style_pad_column(grid, GRID_GAP, 0);
        lv_obj_set_style_pad_row(grid, GRID_GAP, 0);

        const PageDef &page = PAGES[page_index];
        for (size_t button_index = 0; button_index < page.button_count; ++button_index) {
            const ButtonDef *definition = &page.buttons[button_index];
            lv_obj_t *button = theme::make_button(grid, definition->icon, definition->label);
            lv_obj_set_grid_cell(button, LV_GRID_ALIGN_STRETCH, static_cast<lv_coord_t>(button_index % GRID_COLUMNS_COUNT), 1,
                                 LV_GRID_ALIGN_STRETCH, static_cast<lv_coord_t>(button_index / GRID_COLUMNS_COUNT), 1);
            lv_obj_add_event_cb(button, button_event, LV_EVENT_PRESSED, const_cast<ButtonDef *>(definition));
        }
    }

    show_page(0);
    lv_obj_update_layout(screen);
    lv_obj_invalidate(screen);
    lv_timer_create(first_refresh_timer, 200, nullptr);

    last_activity = lv_tick_get();
    // backlight::init() leaves the panel dark; mark the UI dimmed so the first
    // touch (or the end of the splash) explicitly turns it back on.
    dimmed = true;
    lv_timer_create(idle_timer, 1000, nullptr);
}

} // namespace ui
