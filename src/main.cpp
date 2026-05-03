#include <CapacitiveSensor.h>
#include <GyverOLED.h>
#include <Arduino.h>

#define SEND_PIN 52    // pin connected with resistor
#define RECEIVE_PIN 53 // raw sheet pin
#define SEND_PIN2 50
#define RECEIVE_PIN2 51
#define RESTART_BTN 23
#define SELECT_BTN 24

void buttonDb(int btnState, unsigned long lastBtnPress);
void addSolve(float Time);
float calcAo5(float ao5[5]);

CapacitiveSensor cs = CapacitiveSensor(SEND_PIN, RECEIVE_PIN);
CapacitiveSensor cs2 = CapacitiveSensor(SEND_PIN2, RECEIVE_PIN2);

GyverOLED<SSH1106_128x64> oled;

unsigned long sensorStartTime = 0;
unsigned long timerStartTime = 0;
float time = 0.000;
int readyTime = 500;          // 500 ms for hands-on detection
int padsensitivity = 200;     // minimum sensor value to consider a touch
bool canStart = true;

int restartState = 0;
int selectState = 0;
unsigned long lastRestartTime = 0; 
unsigned long lastSelectTime = 0; 
#define DEBOUNCE_DELAY 200  

enum State {
  IDLE,
  STARTING,
  READY,
  TIMING,
};

State currentState = IDLE;

float ao5[5]; // ao5 array
int currentAo5index = 0;
int ao5len = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  pinMode(RESTART_BTN, INPUT);

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
  buttonDb(restartState, lastRestartTime);
  buttonDb(selectState, lastSelectTime);

  if (sensorValue >= padsensitivity && sensorValue2 >= padsensitivity) {
    if (currentState == IDLE && canStart) {
      currentState = STARTING;
      sensorStartTime = millis();
    } else if (currentState == IDLE && !canStart) {
      if (restartState == HIGH) {
        canStart = true;
        time = 0.000;
      }
    }

    if (currentState == STARTING && millis() - sensorStartTime >= readyTime) {
      currentState = READY;
      timerStartTime = millis(); 
    }
  }

  if (sensorValue < padsensitivity || sensorValue2 < padsensitivity) {
    if (currentState == STARTING){
      currentState = IDLE;
    }

    if (currentState == READY) {
      currentState = TIMING; 
      timerStartTime = millis(); 
    }

    if (currentState == TIMING) {
      time = (millis() - timerStartTime) / 1000.0;
    }
  }

  if (sensorValue >= padsensitivity && sensorValue2 >= padsensitivity && currentState == TIMING) {
    currentState = IDLE; 
    canStart = false;
  }

  switch (currentState){
    case IDLE:
      oled.setCursorXY(64, 32); 
      oled.print(time, 3);
      oled.setCursorXY(8, 8); 
      if (ao5len == 5){
        oled.print(calcAo5(ao5), 3);
      }
      break;
    case STARTING:
      oled.setCursorXY(64, 32); 
      oled.clear();
      oled.print("STARTING");
      break;
    case READY:
      oled.setCursorXY(64, 32); 
      oled.clear();
      oled.print("READY");
      break;
    case TIMING:
      oled.setCursorXY(64, 32); 
      oled.clear();
      oled.print(time, 3); 
      break;
  }
  oled.update();
  delay(1);
}

void buttonDb(int btnState, unsigned long lastBtnPress){
  if (restartState == HIGH && (millis() - lastBtnPress) > DEBOUNCE_DELAY) {
      Serial.println("BUTTON PRESSED");
      if (time > 0){
        addSolve(time);
      }
      canStart = true;  
      time = 0.000;     
      lastBtnPress = millis(); 
    }
}

void addSolve(float Time){
  if (ao5len < 5){
    ao5len++;
  }
  ao5[currentAo5index] = Time;
  currentAo5index = (currentAo5index + 1) % 5; // number of solves in average.....5
}

float calcAo5(float ao5[5]){
  float slowest = -INFINITY;
  float fastest = INFINITY; 
  int slowestIndex = -1;
  int fastestIndex = -1;
  float mo3[3];
  
  for (int i = 0; i < 5; i++){
    if (ao5[i] > slowest){
      slowest = ao5[i];
      slowestIndex = i;
    }
    if (ao5[i] < fastest){
      fastest = ao5[i];
      fastestIndex = i;
    }
  }

  int j = 0; 
  for (int i = 0; i < 5; i++) {
    if (i != slowestIndex && i != fastestIndex) {
        mo3[j] = ao5[i];
        j++;
        if (j == 3) break;
      }
    }
  
  float sum = mo3[0] + mo3[1] + mo3[2];
  return sum / 3;
}
