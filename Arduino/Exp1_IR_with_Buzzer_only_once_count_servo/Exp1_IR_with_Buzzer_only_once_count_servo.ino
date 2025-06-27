#include <EEPROM.h>
#include <Servo.h>

const int irSensorPin = 2;
const int buzzerPin = 8;
const int EEPROM_ADDR = 0;

bool objectPreviouslyDetected = false;
int detectionCount = 0;

Servo doorServo;

void setup() {
  pinMode(irSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);

  EEPROM.get(EEPROM_ADDR, detectionCount);
  Serial.print("Startup Count: ");
  Serial.println(detectionCount);

  doorServo.attach(9);
  doorServo.write(0);  // Door initially closed
}

void loop() {
  int sensorValue = digitalRead(irSensorPin);

  if (sensorValue == HIGH && !objectPreviouslyDetected) {
    // Object just detected
    digitalWrite(buzzerPin, HIGH);
    Serial.println("Object Detected!");

    detectionCount++;
    EEPROM.put(EEPROM_ADDR, detectionCount);

    Serial.print("Total Detections: ");
    Serial.println(detectionCount);

    digitalWrite(buzzerPin, LOW);

    doorServo.write(90);  // Open the door
    objectPreviouslyDetected = true;
  } 
  else if (sensorValue == LOW && objectPreviouslyDetected) {
    // Object just moved away
    Serial.println("Object Removed. Closing door.");
    doorServo.write(0);  // Close the door
    objectPreviouslyDetected = false;
  }

  delay(100);
}
