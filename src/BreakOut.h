// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

struct Brick {
    uint8_t x;        // x
    uint8_t y;        // y
    uint8_t w;        // width
    uint8_t h;        // height
    uint16_t color;   // color
    boolean destroy;  // active, if true, check this button, else bypass
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

// Draw panel
void drawPanel() {
    M5.Lcd.fillRect(0, 100, 128, 28, TFT_DARKGRAY);
    M5.Lcd.setTextColor(TFT_BLACK, TFT_DARKGRAY);

    M5.Lcd.setFont(0);
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.drawString("Score", 32, 106);

    M5.Lcd.setFont(&YELLOWCRE8pt7b);
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.drawString(String(score), 32, 119);

    M5.Lcd.setFont(0);
    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.drawString("Ball", 76, 106);
    M5.Lcd.drawString("Level", 76, 120);

    M5.Lcd.setFont(0);
    M5.Lcd.setTextDatum(CR_DATUM);
    M5.Lcd.drawString(String(ballCount), 126, 106);
    M5.Lcd.drawString(String(levelCount), 126, 120);
}

// Init wall
void initWall() {
    uint8_t offset = 6;
    uint8_t i      = 0;
    uint8_t j      = 0;
    uint8_t k      = 0;
    uint8_t index = 0;

    for (i = 0; i < BRICK_NUMBER; i++) {
        if (i % 6 == 0) {
            j = 0;
            k++;
            index = rand() % 8;
        }

        brick[i].x = offset + (20 * j);
        brick[i].y = -10 + 10 * k;
        brick[i].w = 15;
        brick[i].h = 8;
        brick[i].color = palette[index];
        brick[i].destroy = false;

        j++;
    }
}

// Draw wall
void drawWall() {
    uint8_t i = 0;

    for (i = 0; i < BRICK_NUMBER; i++) {
        if (brick[i].destroy == false) {
            if ((ball.x >= brick[i].x && ball.x <= brick[i].x + brick[i].w) &&
                ((ball.y >= brick[i].y && ball.y <= brick[i].y + brick[i].h))) {
                brick[i].destroy = true;
                score++;

                ball.dx =
                    map(brick[i].x + brick[i].w - ball.x, 0, brick[i].w, 2, -2);
                ball.dy *= -1;

                M5.Lcd.fillRect(brick[i].x, brick[i].y, brick[i].w, brick[i].h,
                                TFT_BLACK);
                
                if(score % 24 == 0)
                {
                    levelCount++;
                    ballCount++;
                    initWall();
                    drawPanel();

                    return;
                }
            }

            if (brick[i].destroy == false) {
                M5.Lcd.drawRoundRect(brick[i].x, brick[i].y, brick[i].w,
                                     brick[i].h, 2, TFT_DARKGRAY);
                M5.Lcd.fillRect(brick[i].x + 1, brick[i].y + 1, brick[i].w - 2,
                                brick[i].h - 2, brick[i].color);
            }
        }
    }
}

// Init bat
void initBat() {
    bat.w = 24;
    bat.x = (M5.Displays(0).width() / 2) - (bat.w / 2);
    bat.y = 90;
    bat.h = 2;
    bat.p = bat.x;

    M5.Lcd.drawRect(0, bat.y, 128, bat.h, TFT_BLACK);
    M5.Lcd.drawRect(bat.x, bat.y, bat.w, bat.h, TFT_WHITE);
}

// Draw bat
void drawBat() {
    if (bat.p != bat.x) {
        M5.Lcd.drawRect(bat.p, bat.y, bat.w, bat.h, TFT_BLACK);
        bat.p = bat.x;
        M5.Lcd.drawRect(bat.x, bat.y, bat.w, bat.h, TFT_WHITE);
    }
}

// Init ball
void initBall() {
    boolean btn = true;

    ball.x  = (M5.Displays(0).width() / 2) - 1;
    ball.y  = (M5.Displays(0).width() / 2) - 1;
    ball.w  = 2;
    ball.dx = 0;
    ball.dy = 1;

    M5.Lcd.fillCircle(ball.x, ball.y, ball.w, TFT_WHITE);

    while (btn == true) {
        btn = sensor.getButtonStatus();
        delay(100);
    }
}

// Draw ball
void drawBall() {
    M5.Lcd.fillCircle(ball.x, ball.y, ball.w, TFT_BLACK);

    ball.x += ball.dx;
    ball.y += ball.dy;

    if (ball.y <= 2)  // Up
    {
        ball.dy *= -1;
    } else if (ball.y > 87)  // Down
    {
        initBall();
    }

    else if (ball.x <= 2)  // Left
    {
        ball.dx *= -1;
    } else if (ball.x >= 126)  // Right
    {
        ball.dx *= -1;
    }

    if (ball.y == 87) {
        if (ball.x >= bat.x && ball.x <= bat.x + bat.w) {
            ball.dx = map(bat.x + bat.w - ball.x, 0, bat.w, 3, -3);
            ball.dy *= -1;

        } else {
            ballCount--;
            if (ballCount < 0) {
                ballCount  = 3;
                levelCount = 1;
                score      = 0;

                M5.Lcd.setTextColor(TFT_WHITE);

                M5.Lcd.setFont(&YELLOWCRE8pt7b);
                M5.Lcd.setTextDatum(CC_DATUM);
                M5.Lcd.drawString("GAME OVER", 64, 70);

                boolean btn = true;
                while (btn == true) {
                    btn = sensor.getButtonStatus();
                    delay(100);
                }
                M5.Lcd.clear();
                initWall();
                drawWall();
            }

            drawPanel();
            initBat();
            initBall();
        }
    }

    M5.Lcd.fillCircle(ball.x, ball.y, ball.w, TFT_WHITE);
}