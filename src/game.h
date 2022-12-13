#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "ssd1306.h"

#define JOYSTICK_X_AXIS ADC1_CHANNEL_3
#define JOYSTICK_Y_AXIS ADC1_CHANNEL_0

typedef struct Item{
    int x_pos;
    int y_pos;
    int x_prev;
    int y_prev;
    int x_next;
    int y_next;

	int radius;
}Item;

typedef struct {
    int x0;
    int x1;
    int y0;
    int y1;
}Frame_t;

typedef struct {
    Frame_t frame;

    Item player;
    Item enemy_one;
    Item heart;

    uint8_t win;
    uint8_t screamer;
}Game_t;

SSD1306_t dev;
Game_t game;

void start();