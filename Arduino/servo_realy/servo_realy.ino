#include <Servo.h>

Servo myservo;

#define RELAY1 7
#define RELAY2 8
#define RELAY3 9
#define RELAY4 10
#define SERVO_PIN 6

void setup() {
  myservo.attach(SERVO_PIN);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  // Start with all relays off
  digitalWrite(RELAY1, HIGH); // Most relay boards are LOW-active
  digitalWrite(RELAY2, HIGH);
  digitalWrite(RELAY3, HIGH);
  digitalWrite(RELAY4, HIGH);
}

void loop() {
  // Example: Turn on relay 1 and unlock servo
  digitalWrite(RELAY1, LOW); // ON
  myservo.write(90); // Unlock

  delay(2000);

  digitalWrite(RELAY1, HIGH); // OFF
  myservo.write(0); // Lock

  delay(2000);
}
