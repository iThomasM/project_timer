#include <CapacitiveSensor.h>
#include <GyverOLED.h>

#define SEND_PIN 52    // pin connected with resistor
#define RECEIVE_PIN 53 // raw sheet pin
#define SEND_PIN2 50
#define RECEIVE_PIN2 51

CapacitiveSensor cs = CapacitiveSensor(SEND_PIN, RECEIVE_PIN);
CapacitiveSensor cs2 = CapacitiveSensor(SEND_PIN2, RECEIVE_PIN2);

GyverOLED<SSH1106_128x64> oled;

unsigned long sensorStartTime = 0;
unsigned long timerStartTime = 0;
float time = 0.000;
int readyTime = 500;          // 500 ms for hands-on detection
int padsensitivity = 200;     // Minimum sensor value to consider a touch

enum State {
  IDLE,
  STARTING,
  READY,
  TIMING,
};

State currentState = IDLE;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");

  oled.init();
  oled.clear();
  oled.update();
  oled.setCursorXY(30, 30);
  delay(1000);
}

void loop() {
  long sensorValue = cs.capacitiveSensor(1000); 
  long sensorValue2 = cs2.capacitiveSensor(1000); 
  Serial.print("Sensor 1 Value: ");
  Serial.println(sensorValue);
  Serial.print("Sensor 2 Value: ");
  Serial.println(sensorValue2);

  if (sensorValue >= padsensitivity && sensorValue2 >= padsensitivity) {
    if (currentState == IDLE) {
      currentState = STARTING;
      sensorStartTime = millis();
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
  }

  switch (currentState){
    case IDLE:
      oled.setCursorXY(30, 50); 
      oled.clear();
      oled.print(time, 3);
      break;
    case STARTING:
      oled.setCursorXY(30, 50); 
      oled.clear();
      oled.print("STARTING");
      break;
    case READY:
      oled.setCursorXY(30, 50); 
      oled.clear();
      oled.print("READY");
      break;
    case TIMING:
      oled.setCursorXY(30, 50); 
      oled.clear();
      oled.print(time, 3); 
      break;
  }

  oled.update();
  delay(1);
}