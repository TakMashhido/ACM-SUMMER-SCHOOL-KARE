#include <Servo.h>
#include <IRremote.h>

// Pin definitions
#define SERVO_PIN 6
#define IR_RECEIVE_PIN 7

// Replace this with your actual IR remote button code!
#define PRESS_BUTTON_CODE 0xA956FE01  // Example

Servo myservo;
IRrecv irrecv(IR_RECEIVE_PIN);

void setup() {
  myservo.attach(SERVO_PIN);
  myservo.write(0);  // Start with arm retracted
  irrecv.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.begin(9600);
}

void loop() {
  if (irrecv.decode()) {
    unsigned long codeValue = irrecv.decodedIRData.decodedRawData;

    // Ignore repeat and invalid codes
    if (codeValue == 0xFFFFFFFF || codeValue == 0x0) {
      irrecv.resume();
      return;
    }

    Serial.print("IR Code: 0x");
    Serial.println(codeValue, HEX);

    if (codeValue == PRESS_BUTTON_CODE) {
      // Move servo to "press" the button
      myservo.write(90);  // Adjust to correct angle to press the button
      delay(1000);        // Hold for 1 second (long enough to press)
      myservo.write(0);   // Retract arm
      delay(500);         // Pause before accepting next command
    }

    irrecv.resume();
  }
}
