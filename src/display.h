#ifndef DISPLAY_H
#define DISPLAY_H

#include <GyverOLED.h>
#include <Arduino.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

extern GyverOLED<SSH1106_128x64> oled;

void displayIdle(float time, float ao5[], int ao5len, int currentSelection);
void displayAo5Stage(float ao5[], int ao5len);
void displayCurrentTimeStage(float time, int penaltySelection);
void displayStarting();
void displayReady();
void displayTiming(float time);
float calcAo5(float ao5[5]);

enum Select {
    None,
    Ao5,
    CurrentTime
};

enum Select2 {
    plus2,
    DNF,
    Del,
    OK
};

extern int screenWidth;
extern int screenHeight;
extern int scale;

#endif