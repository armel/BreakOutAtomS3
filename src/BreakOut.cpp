// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#include "BreakOut.h"
#include "functions.h"

void setup() {
    // Init M5
    auto cfg = M5.config();

    cfg.clear_display = true;   // default=true. clear the screen when begin.
    cfg.output_power  = false;  // default=true. use external port 5V output.
    cfg.internal_imu  = true;   // default=true. use internal IMU.
    cfg.internal_rtc  = true;   // default=true. use internal RTC.
    cfg.internal_spk  = true;   // default=true. use internal speaker.
    cfg.internal_mic  = true;   // default=true. use internal microphone.
    cfg.external_imu  = false;  // default=false. use Unit Accel & Gyro.
    cfg.external_rtc  = false;  // default=false. use Unit RTC.

    cfg.external_display.module_display =
        false;  // default=true. use ModuleDisplay
    cfg.external_display.atom_display = true;   // default=true. use AtomDisplay
    cfg.external_display.unit_oled    = false;  // default=true. use UnitOLED
    cfg.external_display.unit_lcd     = false;  // default=true. use UnitLCD
    cfg.external_display.unit_rca =
        false;  // default=false. use UnitRCA VideoOutput
    cfg.external_display.module_rca =
        false;  // default=false. use ModuleRCA VideoOutput

    M5.begin(cfg);

    // Sensor
    M5.Ex_I2C.begin();
    sensor.begin();
    encoderCurrent = sensor.getEncoderValue();

    // AtomS3 Lcd
    display.begin();
    display.setBrightness(64);
    display.clear();

    // Init scroll
    pos = 128;
    img.createSprite(pos, 10);

    // Let's go
    drawPanel();

    initWall();
    drawWall();

    initBat();
    initBall(true);

    // Init Random
    esp_random();
}

void loop() {
    bool btn;

    static int16_t encoderOld = encoderCurrent;
    static uint16_t scoreOld  = score;

    // Manage pause
    btn = sensor.getButtonStatus();

    if (btn == false) {
        display.setFont(&YELLOWCRE8pt7b);
        display.setTextDatum(CC_DATUM);
        display.setTextColor(TFT_WHITE, TFT_BLACK);
        display.drawString("PAUSE", 64, 64);

        delay(500);
        while (true) {
            scroll();
            btn = sensor.getButtonStatus();
            if (btn == false) {
                delay(500);
                display.fillRect(0, 42, 128, 40, TFT_BLACK);
                drawBall();
                break;
            }
            delay(50);
        }
    }

    // Get Encoder value
    encoderCurrent = sensor.getEncoderValue();
    if (encoderOld != encoderCurrent) {
        if (encoderOld > encoderCurrent) {
            bat.x += 2;
            if (bat.x >= (128 - bat.w - 2)) bat.x = 128 - bat.w - 2;
        } else if (encoderOld < encoderCurrent) {
            bat.x -= 2;
            if (bat.x <= 2) bat.x = 2;
        }
        encoderOld = encoderCurrent;
    }

    // Draw stuff
    drawWall();
    drawBat();
    drawBall();

    // Update score
    if (scoreOld != score) {
        scoreOld = score;
        sensor.setLEDColor(0, 0x001100);
        sensor.setLEDColor(1, 0x001100);
        display.setFont(&YELLOWCRE8pt7b);
        display.setTextDatum(CC_DATUM);
        display.setTextColor(TFT_BLACK, TFT_DARKGRAY);
        display.drawString(String(score), 32, 119);
        doPause();
    } else {
        doPause();
    }

    sensor.setLEDColor(0, 0x000000);
    sensor.setLEDColor(1, 0x000000);
}