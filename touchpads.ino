8#include <CapacitiveSensor.h>

// Define the pin numbers
#define SEND_PIN 52    // pin connected with resistor
#define RECEIVE_PIN 53 // raw sheet pin
#define SEND_PIN2 50
#define RECEIVE_PIN2 51

// Create a CapacitiveSensor object
CapacitiveSensor cs = CapacitiveSensor(SEND_PIN, RECEIVE_PIN);
CapacitiveSensor cs2 = CapacitiveSensor(SEND_PIN2, RECEIVE_PIN2);

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  delay(1000);  // Give some time for the sensor to stabilize
}

void loop() {
  long sensorValue = cs.capacitiveSensor(1000); 
  long sensorValue2 = cs2.capacitiveSensor(1000); 

  // Print the sensor value to Serial Monitor
  Serial.print("Sensor 1 Value: ");
  Serial.println(sensorValue);

  Serial.print("Sensor 2 Value: ");
  Serial.println(sensorValue2);

  delay(50);  // Small delay to stabilize readings
}