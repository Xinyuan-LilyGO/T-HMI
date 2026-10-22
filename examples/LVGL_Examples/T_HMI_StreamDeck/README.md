# T-HMI USB Stream Deck

A configurable USB Stream Deck built for the LilyGo T-HMI (ESP32-S3).
The 320x240 touchscreen shows pages of action buttons; each press sends a
USB HID media or keyboard shortcut to the connected computer.

## Features

- 320x240 LVGL interface in landscape orientation
- 4 pages with a 3x2 button grid (6 buttons per page)
- USB HID media keys and keyboard shortcuts through TinyUSB
- Touch navigation with previous/next page arrows
- Config-driven pages and button actions
- Backlight idle dimming after 30 seconds
- Branded splash screen

## Included Pages

| Page | Actions |
| --- | --- |
| Media | Play/Pause, Next, Previous, Volume Up/Down, Mute |
| OBS | Scene 1-3, Mic, Stream, Replay (F13-F18) |
| Meetings | Mic, Camera, Hand, React, Leave, Emoji |
| System | Snip, Copy, Paste, Undo, Desktop, Lock |

OBS buttons use F13-F18 so they can be bound as hotkeys without conflicting
with normal keyboard shortcuts.

## Hardware

- LilyGo T-HMI
- ESP32-S3 with PSRAM
- ST7789 240x320 display (used in 320x240 landscape mode)
- XPT2046 resistive touch controller

## Build and Flash

This is a PlatformIO project.

```sh
pio run -e lilygo_t_hmi
pio run -e lilygo_t_hmi -t upload
```

If the serial port does not enter download mode automatically, hold **BOOT**
and tap **RST**, then upload again.

The USB HID interface requires `ARDUINO_USB_MODE=0`; this is already set in
`platformio.ini`. The board appears as a composite CDC + HID device in this
mode.

## Configuration

Edit `include/config.h` to change pages, labels, icons, and actions.

```cpp
constexpr ButtonDef MEDIA_BUTTONS[] = {
    {"Play", LV_SYMBOL_PLAY, Media(CONSUMER_PLAY_PAUSE)},
    // ...
};

inline constexpr PageDef PAGES[] = {
    make_page("Media", MEDIA_BUTTONS),
    // ...
};
```

- `Media(...)` sends a USB consumer-control action.
- `Keys(MOD_..., HID_KEY_...)` sends a keyboard shortcut.
- Up to 6 buttons per page and up to 8 pages are supported.

## Controls

- Tap a button to send its configured action.
- Tap the left or right arrow in the header to change pages.
- The display dims after 30 seconds of inactivity; touch wakes it.

## Project Structure

```text
include/          Board configuration, actions, and LVGL config
lib/              Vendored XPT2046 touch library
src/              Application, HID manager, backlight, and UI
test/             Host-side configuration validation tests
plans/            Implementation notes
```
