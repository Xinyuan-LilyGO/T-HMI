#include <unity.h>

#include <cstring>

#include "config.h"

namespace {

void assert_page_layout(const PageDef &page) {
    TEST_ASSERT_NOT_NULL(page.title);
    TEST_ASSERT_TRUE(page.button_count > 0);
    TEST_ASSERT_TRUE(page.button_count <= MAX_BUTTONS_PER_PAGE);
    for (size_t i = 0; i < page.button_count; ++i) {
        TEST_ASSERT_NOT_NULL(page.buttons[i].label);
        TEST_ASSERT_NOT_NULL(page.buttons[i].icon);
    }
}

} // namespace

void test_pages_exist(void) {
    TEST_ASSERT_TRUE(PAGE_COUNT > 0);
    TEST_ASSERT_TRUE(PAGE_COUNT <= MAX_PAGES);
}

void test_page_titles_are_unique(void) {
    for (size_t i = 0; i < PAGE_COUNT; ++i) {
        for (size_t j = i + 1; j < PAGE_COUNT; ++j) {
            TEST_ASSERT_TRUE(std::strcmp(PAGES[i].title, PAGES[j].title) != 0);
        }
    }
}

void test_layout_rejects_invalid_pages(void) {
    constexpr PageDef empty_page{nullptr, {}, 0};
    TEST_ASSERT_FALSE(layout_is_valid(empty_page));

    constexpr PageDef missing_title{{}, {{"A", "B", Keys(MOD_CTRL, HID_KEY_A)}}, 1};
    TEST_ASSERT_FALSE(layout_is_valid(missing_title));

    constexpr PageDef missing_label{"Test", {{nullptr, "B", Keys(MOD_CTRL, HID_KEY_A)}}, 1};
    TEST_ASSERT_FALSE(layout_is_valid(missing_label));

    constexpr PageDef missing_icon{"Test", {{"A", nullptr, Keys(MOD_CTRL, HID_KEY_A)}}, 1};
    TEST_ASSERT_FALSE(layout_is_valid(missing_icon));
}

void test_every_page_has_a_valid_layout(void) {
    for (const PageDef &page : PAGES) {
        assert_page_layout(page);
    }
}

void test_every_button_has_an_action(void) {
    for (const PageDef &page : PAGES) {
        for (size_t i = 0; i < page.button_count; ++i) {
            TEST_ASSERT_TRUE(action_is_valid(page.buttons[i].action));
        }
    }
}

void test_modifier_masks(void) {
    TEST_ASSERT_TRUE(is_valid_mod_mask(MOD_CTRL | MOD_SHIFT | MOD_ALT | MOD_GUI));
    TEST_ASSERT_FALSE(is_valid_mod_mask(0x10));
}

void test_action_helpers(void) {
    constexpr ButtonAction key = Keys(static_cast<uint8_t>(MOD_CTRL | MOD_SHIFT), HID_KEY_M);
    TEST_ASSERT_TRUE(action_has_key(key));
    TEST_ASSERT_TRUE(action_is_valid(key));
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(MOD_CTRL | MOD_SHIFT), key.key.mods);
    TEST_ASSERT_EQUAL_UINT8(HID_KEY_M, key.key.keycode);
    TEST_ASSERT_EQUAL_UINT16(CONSUMER_NONE, key.consumer);

    constexpr ButtonAction media = Media(CONSUMER_PLAY_PAUSE);
    TEST_ASSERT_FALSE(action_has_key(media));
    TEST_ASSERT_TRUE(action_is_valid(media));
    TEST_ASSERT_EQUAL_UINT16(CONSUMER_PLAY_PAUSE, media.consumer);
}

void test_invalid_actions_are_rejected(void) {
    constexpr ButtonAction empty = {{MOD_NONE, 0}, CONSUMER_NONE};
    TEST_ASSERT_FALSE(action_is_valid(empty));

    constexpr ButtonAction bad_modifier = {{0x10, 0}, CONSUMER_NONE};
    TEST_ASSERT_FALSE(action_is_valid(bad_modifier));

    constexpr ButtonAction bad_modifier_with_media = {{0x10, 0}, CONSUMER_PLAY_PAUSE};
    TEST_ASSERT_FALSE(action_is_valid(bad_modifier_with_media));

    constexpr ButtonAction modifier_only = {{MOD_CTRL, 0}, CONSUMER_NONE};
    TEST_ASSERT_TRUE(action_is_valid(modifier_only));
}

void test_page_text_is_present(void) {
    for (const PageDef &page : PAGES) {
        TEST_ASSERT_TRUE(std::strlen(page.title) > 0);
        for (size_t i = 0; i < page.button_count; ++i) {
            TEST_ASSERT_TRUE(std::strlen(page.buttons[i].label) > 0);
            TEST_ASSERT_TRUE(std::strlen(page.buttons[i].icon) > 0);
        }
    }
}

void test_config_is_consistent(void) {
    TEST_ASSERT_TRUE(config_is_valid());
}

void setup(void) {
    UNITY_BEGIN();
    RUN_TEST(test_pages_exist);
    RUN_TEST(test_page_titles_are_unique);
    RUN_TEST(test_layout_rejects_invalid_pages);
    RUN_TEST(test_every_page_has_a_valid_layout);
    RUN_TEST(test_every_button_has_an_action);
    RUN_TEST(test_modifier_masks);
    RUN_TEST(test_action_helpers);
    RUN_TEST(test_invalid_actions_are_rejected);
    RUN_TEST(test_page_text_is_present);
    RUN_TEST(test_config_is_consistent);
    UNITY_END();
}

void loop(void) {}
