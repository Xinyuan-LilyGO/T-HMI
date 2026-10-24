# T-HMI USB Stream Deck

A configurable USB Stream Deck for the LilyGo T-HMI (ESP32-S3). The 320x240
touchscreen shows pages of buttons, and each press sends a USB HID media key or
keyboard shortcut to the connected computer.

This project is ready to build with PlatformIO and is intended for anyone who
wants a small, customizable shortcut panel for media control, OBS, meetings, or
desktop automation.

## What You Need

- LilyGo T-HMI board
- USB-C data cable (not a charge-only cable)
- Windows, macOS, or Linux computer
- PlatformIO Core or the PlatformIO IDE extension for VS Code

Tested with:

- PlatformIO `espressif32@6.9.0`
- LVGL `8.3.11`
- LilyGo T-HMI

## Features

- 320x240 LVGL interface in landscape orientation
- 4 pages with a 3x2 button grid (6 buttons per page)
- USB HID media keys and keyboard shortcuts through TinyUSB
- Touch navigation with previous/next page arrows
- Config-driven pages and button actions
- Backlight idle dimming after 30 seconds
- Branded splash screen
- Compile-time and boot-time configuration validation

## Included Pages

| Page | Buttons |
| --- | --- |
| Media | Play/Pause, Next, Previous, Volume Up, Volume Down, Mute |
| OBS | Scene 1, Scene 2, Scene 3, Mic, Stream, Replay |
| Meetings | Mic, Camera, Hand, React, Leave, Emoji |
| System | Snip, Copy, Paste, Undo, Desktop, Lock |

The OBS buttons send `F13` through `F18`. Bind those keys inside OBS under
**Settings > Hotkeys** so they do not conflict with normal keyboard shortcuts.

The meeting buttons use common Microsoft Teams / Zoom / Google Meet shortcuts,
but these can vary by app and version. Remap them in `include/config.h` if your
app uses different shortcuts.

## Quick Start

1. Connect the T-HMI to your computer with a USB-C data cable.
2. Build and flash the project (see below).
3. Wait for the splash screen to finish.
4. Tap any button to send its action to the computer.
5. Use the left and right header arrows to change pages.

The screen dims after 30 seconds of inactivity. Tap the screen to wake it.

## Build and Flash

This is a PlatformIO project.

### Option 1: PlatformIO Core

Install PlatformIO if needed:

```sh
pip install platformio
```

Build from the project folder:

```sh
pio run -e lilygo_t_hmi
```

Upload to the board:

```sh
pio run -e lilygo_t_hmi -t upload
```

If your board is on a specific port, list the ports first:

```sh
pio device list
```

Then upload with the port explicitly:

```sh
pio run -e lilygo_t_hmi -t upload --upload-port COM5
```

Replace `COM5` with your board's port (for example `/dev/ttyACM0` on Linux or
`/dev/cu.usbmodem...` on macOS).

### Option 2: VS Code

1. Install the PlatformIO IDE extension.
2. Open this folder in VS Code.
3. Use **PlatformIO > Build** to compile.
4. Use **PlatformIO > Upload** to flash the board.

### If Upload Fails

If the upload tool cannot connect:

1. Hold the **BOOT** button.
2. Tap the **RST** button once.
3. Release **BOOT**.
4. Run the upload command again.

On Windows, the board can expose two serial ports. Try the other port if the
first one does not respond. The bootloader port is usually the one that appears
after the BOOT + RST sequence.

## How It Works

The board enumerates as a composite USB CDC + HID device. The touchscreen sends
button presses to the ESP32-S3, and the firmware sends the matching HID report
to the host computer.

The USB HID interface requires `ARDUINO_USB_MODE=0`. This is already set in
`platformio.ini`. Do not change it unless you know you need a different USB
mode, because HID reports will not work in USB-Serial-JTAG mode.

## Using the Deck

- Tap a button to send its configured action.
- Tap the left arrow to go to the previous page.
- Tap the right arrow to go to the next page.
- Page navigation wraps around from the last page to the first page.
- The screen dims after 30 seconds without touch input.

The deck does not require any driver on Windows, macOS, or Linux for standard
media keys and keyboard shortcuts. The host treats it as a normal USB keyboard
and consumer-control device.

## Testing

The configuration tests live in `test/test_config`.

Compile-check them without uploading or running:

```sh
pio test -e lilygo_t_hmi --without-uploading --without-testing
```

Run them on a connected board:

```sh
pio test -e lilygo_t_hmi
```

Running the tests flashes a test firmware. Reflash the main application
afterward.

## Customizing Buttons and Pages

All page and button definitions live in `include/config.h`.

Each button has:

```cpp
struct ButtonDef {
    const char *label;
    const char *icon;
    ButtonAction action;
};
```

Example button table:

```cpp
constexpr ButtonDef MY_BUTTONS[] = {
    {"Mute",  LV_SYMBOL_MUTE,  Media(CONSUMER_MUTE)},
    {"Save",  LV_SYMBOL_SAVE,  Keys(MOD_CTRL, HID_KEY_S)},
    {"Task",  LV_SYMBOL_LIST,  Keys(MOD_CTRL | MOD_SHIFT, HID_KEY_ESCAPE)},
};
```

Add the page to `PAGES`:

```cpp
inline constexpr PageDef PAGES[] = {
    make_page("Media", MEDIA_BUTTONS),
    make_page("Custom", MY_BUTTONS),
};
```

`PAGE_COUNT` is calculated automatically. The current limits are:

- Up to 6 buttons per page
- Up to 8 pages

## Action Types

### Media Actions

Use `Media(...)` for consumer-control actions such as play/pause and volume.

```cpp
Media(CONSUMER_PLAY_PAUSE)
Media(CONSUMER_SCAN_NEXT)
Media(CONSUMER_SCAN_PREV)
Media(CONSUMER_VOL_UP)
Media(CONSUMER_VOL_DOWN)
Media(CONSUMER_MUTE)
```

### Keyboard Actions

Use `Keys(modifiers, keycode)` for keyboard shortcuts.

```cpp
Keys(MOD_CTRL, HID_KEY_C)                 // Ctrl+C
Keys(MOD_CTRL | MOD_SHIFT, HID_KEY_M)    // Ctrl+Shift+M
Keys(MOD_GUI, HID_KEY_D)                  // Windows/Command+D
Keys(MOD_NONE, HID_KEY_F13)               // F13
```

Available modifiers:

| Modifier | Meaning |
| --- | --- |
| `MOD_NONE` | No modifier |
| `MOD_CTRL` | Ctrl |
| `MOD_SHIFT` | Shift |
| `MOD_ALT` | Alt |
| `MOD_GUI` | Windows / Command |

Key names use TinyUSB `HID_KEY_*` constants, such as `HID_KEY_A`,
`HID_KEY_SPACE`, `HID_KEY_ENTER`, or `HID_KEY_F13`.

### Icons and Labels

Icons use LVGL symbols such as:

```cpp
LV_SYMBOL_PLAY
LV_SYMBOL_PAUSE
LV_SYMBOL_NEXT
LV_SYMBOL_PREV
LV_SYMBOL_PLUS
LV_SYMBOL_MINUS
LV_SYMBOL_MUTE
LV_SYMBOL_VIDEO
LV_SYMBOL_AUDIO
LV_SYMBOL_IMAGE
LV_SYMBOL_COPY
LV_SYMBOL_PASTE
LV_SYMBOL_HOME
LV_SYMBOL_SETTINGS
```

Labels are plain text and are displayed under the icon.

## OBS Setup

The OBS page sends `F13` through `F18`. To use it:

1. Open OBS.
2. Go to **Settings > Hotkeys**.
3. Find the action you want to control.
4. Click its hotkey field and press the matching key on a normal keyboard, or
   use OBS's key capture after connecting the deck.
5. Repeat for each scene, mic, stream, and replay action.

## Changing Colors and Styling

The visual theme is in:

- `src/ui/theme.h` for the palette
- `src/ui/theme.cpp` for button, label, and header styles
- `src/ui/ui_manager.cpp` for the page layout and header

The current theme uses a near-black background, lime-green accent, and flat
rounded cards with no shadows.

## Project Structure

```text
include/          Configuration, HID actions, backlight, and LVGL config
lib/              Vendored XPT2046 touch library
src/main.cpp      Display, touch, LVGL, and application startup
src/hid_manager.* USB HID keyboard and consumer-control output
src/backlight.*   Backlight control and idle dimming
src/ui/           Theme, layout, page switching, and shared UI state
test/             On-target configuration validation tests
plans/            Implementation notes
```

## Troubleshooting

### Upload cannot connect

Put the board into bootloader mode: hold **BOOT**, tap **RST**, release
**BOOT**, then upload again.

### Board is detected on two COM ports

Use the port that responds during upload. If one port fails, try the other. The
bootloader port is usually the one present after the BOOT + RST sequence.

### Display is dark

The backlight dims after 30 seconds of inactivity. Tap the screen to wake it.

### Touch is inaccurate

Touch calibration is configured in `src/main.cpp`:

```cpp
touch.setCal(1788, 285, 1877, 311, PANEL_WIDTH, PANEL_HEIGHT);
touch.setRotation(TOUCH_ROTATION);
```

The firmware can print `[TOUCH]` diagnostics over serial when a touch starts.
Enable them by changing `-D TOUCH_DEBUG=0` to `-D TOUCH_DEBUG=1` in
`platformio.ini`, rebuild, and open the serial monitor:

```sh
pio device monitor --baud 115200
```

Use the reported coordinates to adjust the calibration values or rotation.

### HID actions do not reach the computer

- Use a USB-C data cable, not a charge-only cable.
- Confirm the board is connected directly to the computer or through a powered
  USB hub.
- Check the serial monitor for `[HID] USB host not ready`.
- Make sure `ARDUINO_USB_MODE=0` is still set in `platformio.ini`.

### Buttons send the wrong shortcut

Edit `include/config.h` and adjust the `Keys(...)` or `Media(...)` action for
that button. Rebuild and flash the board.

## Safety and Security

This is a local USB HID device. It has no network stack, storage, or secrets.
Treat it like a keyboard: anyone with physical USB access can trigger the
configured shortcuts, so do not bind destructive actions to buttons that are
easy to press accidentally.

The default configuration includes `Win+L` on the System page. Move or remove
that action if accidental lockouts are a concern.

## Notes

- The display is configured for landscape mode using MADCTL `0xA0`.
- The touch controller uses rotation `1` and the calibration values in
  `src/main.cpp`.
- The project uses the LilyGo T-HMI pinout and is not intended as a generic
  ESP32-S3 display example without checking the wiring.
