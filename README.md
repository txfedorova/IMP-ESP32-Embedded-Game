# IMP / ESP32 Embedded Game

Embedded systems project implemented in C for an ESP32 development board. The application runs a small real-time game on a 128×64 SSD1306 OLED display and uses an analog joystick for player input.

## Project overview

The game runs on ESP-IDF with FreeRTOS. The player is controlled through ADC joystick readings and moves inside the display area while interacting with two moving objects: an enemy and a heart. Reaching the heart triggers the win state; colliding with the enemy triggers the lose state and displays a bitmap animation.

The implementation separates display/game updates and joystick input into independent FreeRTOS tasks.

## Main features

- embedded C application for ESP32
- SSD1306 128×64 OLED output over I2C
- analog joystick input through ESP32 ADC
- FreeRTOS tasks for input and game/display updates
- player, enemy and collectible movement
- collision detection and game-state handling
- wrap-around movement at display boundaries
- custom pixel graphics and bitmap rendering
- win and lose sequences on the OLED display

## Implementation

The application starts in `app_main()` by initializing the SSD1306 display and ADC input, then calls the game initialization routine.

Two FreeRTOS tasks handle the runtime logic:

- `joystick` reads the ADC values and converts them into player movement increments.
- `display` updates the player, enemy and heart positions, checks collisions and redraws the game objects.

Game state is stored in `Game_t`, which contains the screen frame, player, enemy and heart objects together with win/lose state flags.

## Hardware and software

The project is configured for:

- ESP32 development board (`esp32dev`)
- ESP-IDF framework
- SSD1306 OLED display
- analog joystick
- PlatformIO

The SSD1306 driver is included under `components/ssd1306/`.

## Build

The repository contains a PlatformIO configuration for the ESP32 development board:

```bash
pio run
```

To build and upload to a connected ESP32 board:

```bash
pio run -t upload
```

Serial monitoring is configured at 115200 baud:

```bash
pio device monitor
```

Hardware-specific SDA, SCL and reset pin configuration is handled through the project configuration used by the SSD1306 component.

## Repository structure

```text
.
├── README.md
├── platformio.ini
├── CMakeLists.txt
├── components/
│   └── ssd1306/
├── src/
│   ├── main.c
│   ├── game.c
│   ├── game.h
│   └── CMakeLists.txt
├── include/
├── lib/
└── test/
```

- `src/main.c` — ESP32 application entry point and hardware initialization
- `src/game.c` — game rendering, movement, collision detection and FreeRTOS tasks
- `src/game.h` — game data structures and hardware-related declarations
- `components/ssd1306/` — OLED display component
- `platformio.ini` — ESP32 / ESP-IDF PlatformIO configuration
