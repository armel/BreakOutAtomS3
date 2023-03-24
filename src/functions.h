// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

// Draw console (for debug)
void console(String value, uint8_t x, uint8_t y) {
    display.setFont(0);
    display.setTextDatum(CC_DATUM);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setTextPadding(32);
    display.drawString(value, x, y);
}

// Build scroll
void buildScroll() {
    int16_t h = 10;
    int16_t w;

    img.setFont(0);
    w = img.textWidth(message) + 40;
    // We could just use fillSprite(color) but lets be a bit more creative...
    while (h--) img.drawFastHLine(0, h, w, TFT_BLACK);

    // Now print text on top of the graphics
    img.setTextColor(TFT_WHITE);  // White text, no background colour
    img.setTextWrap(false);  // Turn of wrap so we can print past end of sprite

    // Need to print twice so text appears to wrap around at left and right
    // edges
    img.drawString(message, pos, 1);
    img.drawString(message, pos - w, 1);
}

// Scroll
void scroll() {
    // Sprite for scroll
    buildScroll();
    img.pushSprite(0, 42);

    pos -= 1;
    if (pos < 0) {
        img.setFont(0);
        pos = img.textWidth(message) + 40;
    }
}

// Draw panel
void drawPanel() {
    display.fillRect(0, 100, 128, 28, TFT_DARKGRAY);
    display.setTextColor(TFT_BLACK, TFT_DARKGRAY);

    display.setFont(0);
    display.setTextDatum(CC_DATUM);
    display.setTextPadding(0);
    display.drawString("Score", 32, 106);

    display.setFont(&YELLOWCRE8pt7b);
    display.setTextDatum(CC_DATUM);
    display.setTextPadding(64);
    display.drawString(String(score), 32, 119);
    display.setTextPadding(0);

    display.setFont(0);
    display.setTextDatum(CL_DATUM);
    display.setTextPadding(0);
    display.drawString("Ball", 76, 106);
    display.drawString("Level", 76, 120);

    display.setFont(0);
    display.setTextDatum(CR_DATUM);
    display.setTextPadding(20);
    display.drawString(String(ballCount), 126, 106);
    display.drawString(String(levelCount), 126, 120);

    display.setTextPadding(0);
}

// Init wall
void initWall() {
    uint8_t offset = 6;
    uint8_t i      = 0;
    uint8_t j      = 0;
    uint8_t k      = 0;
    uint8_t index  = 0;

    for (i = 0; i < BRICK_NUMBER; i++) {
        if (i % 6 == 0) {
            j = 0;
            k++;
            index = random(0, 8);
        }

        brick[i].x       = offset + (20 * j);
        brick[i].y       = -10 + 10 * k;
        brick[i].w       = 15;
        brick[i].h       = 8;
        brick[i].color   = palette[index];
        brick[i].destroy = false;

        j++;
    }
}

// Draw wall
void drawWall() {
    uint8_t i = 0;

    for (i = 0; i < BRICK_NUMBER; i++) {
        if (brick[i].destroy == false) {
            if ((ball.x + 1 >= brick[i].x &&
                 ball.x - 1 <= brick[i].x + brick[i].w) &&
                ((ball.y >= brick[i].y && ball.y <= brick[i].y + brick[i].h))) {
                brick[i].destroy = true;
                score++;

                ball.dx =
                    map(brick[i].x + brick[i].w - ball.x, 0, brick[i].w, 2, -2);
                ball.dy *= -1;

                display.fillRect(brick[i].x, brick[i].y, brick[i].w, brick[i].h,
                                 TFT_BLACK);

                if (score % 24 == 0) {
                    levelCount++;
                    ballCount++;
                    initWall();
                    drawPanel();

                    return;
                }
            }

            if (brick[i].destroy == false) {
                display.drawRoundRect(brick[i].x, brick[i].y, brick[i].w,
                                      brick[i].h, 2, TFT_DARKGRAY);
                display.fillRect(brick[i].x + 1, brick[i].y + 1, brick[i].w - 2,
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

    display.drawRect(0, bat.y, 128, bat.h, TFT_BLACK);
    display.drawRect(bat.x, bat.y, bat.w, bat.h, TFT_WHITE);
}

// Draw bat
void drawBat() {
    if (bat.p != bat.x) {
        display.drawRect(bat.p, bat.y, bat.w, bat.h, TFT_BLACK);
        bat.p = bat.x;
        display.drawRect(bat.x, bat.y, bat.w, bat.h, TFT_WHITE);
    }
}

// Init ball
void initBall(bool startup = false) {
    bool btn = true;

    ball.x  = (M5.Displays(0).width() / 2) - 1;
    ball.y  = (M5.Displays(0).width() / 2) - 1;
    ball.w  = 2;
    ball.dx = 0;
    ball.dy = 1;

    display.fillCircle(ball.x, ball.y, ball.w, TFT_WHITE);

    if(startup == true)
    {
        display.setFont(&YELLOWCRE8pt7b);
        display.setTextDatum(CC_DATUM);
        display.setTextColor(TFT_WHITE, TFT_BLACK);
        display.drawString("START", 64, 64);
    }

    while (btn == true) {
        btn = sensor.getButtonStatus();
        if (startup == true) scroll();
        delay(50);
    }

    if(startup == true)
    {
        display.fillRect(0, 42, 128, 40, TFT_BLACK);
    }

    delay(100);
}

// Draw ball
void drawBall() {
    display.fillCircle(ball.x, ball.y, ball.w, TFT_BLACK);

    ball.x += ball.dx;
    ball.y += ball.dy;

    // console(String(ball.dx), 10, 66);
    // console(String(ball.dy), 40, 66);

    if (ball.y <= 2)  // Up
    {
        ball.dx = map(random(0, 8), 0, 7, 3, -3);
        ball.dy = 1;
    } else if (ball.y > 87)  // Down
    {
        initBall();
    }

    else if (ball.x <= 2)  // Left
    {
        ball.dx = abs(ball.dx);
    } else if (ball.x >= 125)  // Right
    {
        ball.dx = -abs(ball.dx);
    }

    if (ball.y == 87) {
        if (ball.x + 1 >= bat.x && ball.x - 1 <= bat.x + bat.w) {
            ball.dx = map(bat.x + bat.w - ball.x, 0, bat.w, 3, -3);
            ball.dy *= -1;

        } else {
            ballCount--;
            if (ballCount < 0) {
                ballCount  = BALL_NUMBER;
                levelCount = 1;
                score      = 0;

                display.setTextColor(TFT_WHITE);

                display.setFont(&YELLOWCRE8pt7b);
                display.setTextDatum(CC_DATUM);
                display.drawString("GAME OVER", 64, 70);

                bool btn = true;
                while (btn == true) {
                    scroll();
                    btn = sensor.getButtonStatus();
                    delay(50);
                }
                display.clear();
                initWall();
                drawWall();
            }

            drawPanel();
            initBat();
            initBall();
        }
    }

    display.fillCircle(ball.x, ball.y, ball.w, TFT_WHITE);
}

// Manage pause
void doPause() {
    uint8_t pause = 0;

    switch (max(abs(ball.dx), abs(ball.dy))) {
        case 1:
            pause = 20;
            break;

        case 2:
            pause = 30;
            break;

        case 3:
            pause = 40;
            break;
    }
    delay(pause);
}