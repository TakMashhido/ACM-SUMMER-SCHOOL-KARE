#include <IRremote.h>
#include <Servo.h>

// Pin assignments
#define IR_RECEIVE_PIN 7
#define SERVO_PIN 6
#define BUZZER_PIN 8

// Set your password sequence here (IR codes, in order)
#define PW_LENGTH 3
unsigned long PASSWORD[PW_LENGTH] = {0xA956FE01, 0xA55AFE01, 0xB946FE01}; // Example: Red, Green, Blue buttons

// State variables
unsigned long entered[PW_LENGTH];
int entered_pos = 0;

Servo myservo;
IRrecv irrecv(IR_RECEIVE_PIN);

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  myservo.attach(SERVO_PIN);
  lock();  // Start locked

  irrecv.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("Locker system ready.");
}

void loop() {
  if (irrecv.decode()) {
    unsigned long codeValue = irrecv.decodedIRData.decodedRawData;
    Serial.print("IR Code: 0x");
    Serial.println(codeValue, HEX);

    // Ignore repeats and invalids
    if (codeValue == 0xFFFFFFFF || codeValue == 0x0) {
      irrecv.resume();
      return;
    }

    // Add this button to entered sequence
    entered[entered_pos] = codeValue;
    entered_pos++;

    // Check if entered sequence is as long as password
    if (entered_pos == PW_LENGTH) {
      bool correct = true;
      for (int i = 0; i < PW_LENGTH; i++) {
        if (entered[i] != PASSWORD[i]) {
          correct = false;
          break;
        }
      }
      if (correct) {
        unlock();
        Serial.println("Unlocked!");
      } else {
        buzz();
        lock();
        Serial.println("Wrong password! Locked.");
      }
      entered_pos = 0;  // Reset input
    }

    irrecv.resume();
  }
}

void lock() {
  myservo.write(0); // adjust as needed for your latch
}

void unlock() {
  myservo.write(90); // adjust as needed for your latch
  delay(5000); // stays unlocked for 5 seconds
  lock();
  Serial.println("Locked again.");
}

void buzz() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(150);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
}
