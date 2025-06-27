#include <Servo.h>
Servo myservo;

#define SERVO_PIN 6

void setup() {
  myservo.attach(SERVO_PIN);
}

void loop() {
  myservo.write(90);
  delay(2000);
  myservo.write(0);
  delay(2000);
}
