// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#define VERSION "0.0.2"
#define AUTHOR "F4HWN"
#define NAME "AtomS3BreakOut"

#define BRICK_NUMBER 24
#define BALL_NUMBER  5

#include <M5Unified.h>
#include "fonts.h"
#include "UnitEncoder.h"

Unit_Encoder sensor;
M5GFX &display(M5.Lcd);

uint16_t palette[] = {TFT_BLUE,   TFT_DARKGREEN, TFT_PINK,   TFT_RED,
                      TFT_ORANGE, TFT_MAROON,    TFT_PURPLE, TFT_NAVY};

int16_t encoderCurrent;
uint16_t score     = 0;
int8_t ballCount   = BALL_NUMBER;
uint8_t levelCount = 1;

struct Brick {
    uint8_t x;       // x
    uint8_t y;       // y
    uint8_t w;       // width
    uint8_t h;       // height
    uint16_t color;  // color
    bool destroy;    // active, if true, check this button, else bypass
};

Brick brick[BRICK_NUMBER];

struct Bat {
    uint8_t x;  // x
    uint8_t y;  // y
    uint8_t w;  // width
    uint8_t h;  // height
    uint8_t p;  // previous x
};

Bat bat;

struct Ball {
    uint8_t x;  // x
    uint8_t y;  // y
    uint8_t w;  // width
    int8_t dx;  // move x
    int8_t dy;  // move y
};

Ball ball;

// Scroll
LGFX_Sprite img(&M5.Display);
String message = String(NAME) + " v" + String(VERSION) + " by " + String(AUTHOR);
int16_t pos;