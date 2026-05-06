#include <CapacitiveSensor.h>
#include <Arduino.h>
#include "display.h"

#define SEND_PIN 52
#define RECEIVE_PIN 53
#define SEND_PIN2 50
#define RECEIVE_PIN2 51
#define RESTART_BTN 23
#define SELECT_BTN 22

#define DEBOUNCE_DELAY 200

void changeState(long sensorValue, long sensorValue2);
void restartBTN(int btnState, unsigned long &lastBtnPress);
void selectBTN(int btnState, unsigned long &lastSelectTime);
void addSolve(float Time);

CapacitiveSensor cs(SEND_PIN, RECEIVE_PIN);
CapacitiveSensor cs2(SEND_PIN2, RECEIVE_PIN2);

unsigned long sensorStartTime = 0;
unsigned long timerStartTime = 0;
float time = 0.000;
int readyTime = 500;
int padsensitivity = 200;
bool canStart = true;

int restartState = 0;
int selectState = 0;
unsigned long lastRestartTime = 0; 
unsigned long lastSelectTime = 0; 

enum State {IDLE, STARTING, READY, TIMING, AO5_STAGE, CURRENT_TIME_STAGE};
State currentState = IDLE;

Select currentSelection = None;
Select2 penaltySelection = OK;

float ao5[5]; 
int currentAo5index = 0;
int ao5len = 0;

void setup() {
    Serial.begin(9600);
    pinMode(RESTART_BTN, INPUT);
    pinMode(SELECT_BTN, INPUT);

    oled.init();
    oled.clear();
    oled.update();
    oled.setCursorXY(30, 30);
    delay(1000);
}

void loop() {
    long sensorValue = cs.capacitiveSensor(1000); 
    long sensorValue2 = cs2.capacitiveSensor(1000); 

    restartState = digitalRead(RESTART_BTN);
    selectState = digitalRead(SELECT_BTN);

    restartBTN(restartState, lastRestartTime);
    selectBTN(selectState, lastSelectTime);
    changeState(sensorValue, sensorValue2);

    switch (currentState) {
        case IDLE: displayIdle(time, ao5, ao5len, currentSelection); break;
        case AO5_STAGE: displayAo5Stage(ao5, ao5len); break;
        case CURRENT_TIME_STAGE: displayCurrentTimeStage(time, penaltySelection); break;
        case STARTING: displayStarting(); break;
        case READY: displayReady(); break;
        case TIMING: displayTiming(time); break;
    }
}

void restartBTN(int btnState, unsigned long &lastBtnPress) {
    if (btnState == HIGH && (millis() - lastBtnPress) > DEBOUNCE_DELAY) {
        if (currentSelection == None && time > 0) {
            addSolve(time);
            canStart = true;
            time = 0.000;
        }

        if (currentState == IDLE) {
            if (currentSelection == Ao5) currentState = AO5_STAGE;
            else if (currentSelection == CurrentTime) currentState = CURRENT_TIME_STAGE;
        }

        // single options
        else if (currentState == CURRENT_TIME_STAGE && time > 0) {
            switch (penaltySelection) {
                case plus2: time += 2.000; break;
                case DNF: time = INFINITY; break;
                case Del: time = 0.000; canStart = true; break;
                case OK: break;
            }
            currentState = IDLE;  
        }
        else if (currentState == AO5_STAGE) {
            currentState = IDLE;
        }
        else {currentState = IDLE;}
        lastBtnPress = millis();
    }
}

void selectBTN(int btnState, unsigned long &lastSelectTime) {
    if (btnState == HIGH && (millis() - lastSelectTime) > DEBOUNCE_DELAY) {
        if (currentState == CURRENT_TIME_STAGE) {
            penaltySelection = (Select2)(((int)penaltySelection + 1) % 4);
            Serial.println(penaltySelection);
        }
        else if (currentState == IDLE) {
            currentSelection = (Select)(((int)currentSelection + 1) % 3);
            Serial.println(currentSelection);
        }
        lastSelectTime = millis(); 
    }
}

void addSolve(float Time) {
    if (ao5len < 5) ao5len++;
    ao5[currentAo5index] = Time;
    currentAo5index = (currentAo5index + 1) % 5;
}

void changeState(long sensorValue, long sensorValue2) {
    if (sensorValue >= padsensitivity && sensorValue2 >= padsensitivity) {
        if (currentState == IDLE && canStart) {
            currentState = STARTING;
            sensorStartTime = millis();
        } else if (currentState == IDLE && !canStart && restartState == HIGH) {
            canStart = true;
            time = 0.000;
        }

        if (currentState == STARTING && millis() - sensorStartTime >= readyTime) {
            currentState = READY;
            timerStartTime = millis(); 
        }
    }

    if (sensorValue < padsensitivity || sensorValue2 < padsensitivity) {
        if (currentState == STARTING) currentState = IDLE;
        if (currentState == READY) currentState = TIMING;
        if (currentState == TIMING) time = (millis() - timerStartTime) / 1000.0;
    }

    if (sensorValue >= padsensitivity && sensorValue2 >= padsensitivity && currentState == TIMING) {
        currentState = IDLE;
        canStart = false;
    }
}