# T-HMI Stream Deck — Implementation Notes

A Stream Deck style macro pad on the **LilyGo T-HMI** (ESP32-S3) with an LVGL
touch UI and native USB HID output. No companion software is required.

## Hardware

| Component | Detail |
|---|---|
| MCU | ESP32-S3, 16 MB flash, 8 MB OPI PSRAM |
| Display | 240x320 ST7789, 8-bit Intel 8080 parallel bus |
| Touch | XPT2046, SPI, interrupt on GPIO 9 |
| Backlight | GPIO 38, LEDC PWM (5 kHz, 8-bit), dims to ~16% after 30 s idle |
| USB | Native USB-OTG, TinyUSB composite: CDC serial + HID keyboard + HID consumer |
| USB mode | `ARDUINO_USB_MODE=0` (TinyUSB). Mode 1 only starts the USB-Serial-JTAG peripheral, so HID never becomes ready. |

Pin map lives in `src/main.cpp`. It matches the official LilyGo T-HMI examples
(`examples/LVGL_Examples/Basic_Button/pins.h`).

## Display and UI

- Panel is natively portrait 240x320. `MADCTL = 0xA0` (MY | MV) matches LilyGo's
  factory landscape mode and rotates it to **320x240 landscape**; LVGL is
  configured for 320x240. `0x60` is the same landscape rotated 180 degrees, so
  both the image and touch end up inverted.
- UI: header (40 px) with page title, direct page tabs, and previous/next page
  buttons. Each page is a 3x2 grid, up to six buttons.
- Pages are stacked views; only the active page is visible. Page changes use
  the header tabs or arrows, so a touch that starts on a button cannot turn
  into a page swipe.
- A 1.5 s "FUSION AUTOMATE" splash is shown before the deck. The backlight is
  kept off until the splash frame is drawn, and the idle timeout starts when the
  splash ends.
- The display flush is asynchronous. `lv_disp_flush_ready()` is called from the
  panel IO `on_color_trans_done` callback, never directly in `flush_cb`.
- Draw buffers are 40 lines each and prefer internal DMA RAM, falling back to
  PSRAM.

## Data flow

```
touch -> LVGL click -> ButtonDef.action (include/config.h)
      -> HIDManager -> TinyUSB keyboard / consumer report
```

`include/actions.h` owns the HID action types; `include/config.h` owns the page
table. Page layout errors are caught by `static_assert`; action errors are
checked once at boot by `config_is_valid()`.

## Pages

1. **Media** — consumer Play/Pause, Next, Prev, Vol +/-, Mute.
2. **OBS** — F13-F18 (bind them in OBS Settings > Hotkeys).
3. **Meetings** — Ctrl+Shift+M/O/P/H, Alt+Y, Win+Period.
4. **System** — Win+Shift+S, Ctrl+C/V/Z, Win+D, Win+L.

Edit `include/config.h` to add or remap buttons. `HID_KEY_*` names from TinyUSB
are used for keycodes so modifiers and letters cannot be confused with one
another.

## Build, flash, test

```powershell
& C:\Python313\python.exe -m platformio run -e lilygo_t_hmi
& C:\Python313\python.exe -m platformio run -e lilygo_t_hmi -t upload
& C:\Python313\python.exe -m platformio device monitor -b 115200
# Compile-check the on-target Unity tests without touching a board:
& C:\Python313\python.exe -m platformio test -e lilygo_t_hmi --without-uploading --without-testing
# Run them on hardware:
& C:\Python313\python.exe -m platformio test -e lilygo_t_hmi
```

Uploading requires the ROM bootloader: hold **BOOT**, tap **RST**, release
**RST**, release **BOOT**. After the upload, release **BOOT** and tap **RST** to
start the application.

## Security model

This is a local USB HID device with compile-time actions. It has no network
stack, storage, or secrets. Anyone with physical USB access can send the
configured shortcuts, so treat it like a keyboard: do not bind destructive
shortcuts to buttons that are easy to press accidentally.

## Known limitations / follow-ups

- HID output requires the board to be connected to a USB **host** that has
  enumerated the HID interfaces. Presses are ignored (and logged once every
  5 s) while the host is not ready.
- Idle dimming uses LEDC PWM on the backlight (GPIO38, channel 0, 5 kHz), not
  LVGL opacity, so dimming never triggers a full-screen redraw. Haptics are not
  implemented.
- Touch calibration uses the example defaults in native portrait order
  (`1788, 285, 1877, 311`) with `TOUCH_ROTATION = 1`. This is equivalent to
  LilyGo's `285, 1788, 311, 1877` with rotation `3`, and pairs with
  `MADCTL = 0xA0`. If touch is still offset, store per-unit calibration values.
- `LV_COLOR_16_SWAP 0` must stay consistent with `swap_color_bytes = 1` in the
  i80 panel IO configuration.
