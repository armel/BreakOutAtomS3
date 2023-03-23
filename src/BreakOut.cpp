// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#define BRICK_NUMBER 24

#include <M5Unified.h>
#include "font.h"
#include "Unit_Encoder.h"

Unit_Encoder sensor;

#include "BreakOut.h"

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

    // M5Stack Lcd
    M5.Lcd.begin();
    M5.Lcd.setBrightness(64);
    M5.Lcd.clear();

    // Let's go
    drawPanel();

    initWall();
    drawWall();

    initBat();
    initBall();
}

void loop() {
    static int16_t encoderOld = encoderCurrent;
    static uint16_t scoreOld  = score;

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
        M5.Lcd.setFont(&YELLOWCRE8pt7b);
        M5.Lcd.setTextDatum(CC_DATUM);
        M5.Lcd.setTextColor(TFT_BLACK, TFT_DARKGRAY);
        M5.Lcd.drawString(String(score), 32, 119);
        delay(25);
    } else {
        delay(25);
    }

    sensor.setLEDColor(0, 0x000000);
    sensor.setLEDColor(1, 0x000000);
}