#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

// Pin definitions
#define IR_RECEIVE_PIN D5   // IR receiver connected to D5 (GPIO14)
#define LED_PIN D2          // LED connected to D2 (GPIO4)

IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;

// Variables for long press detection
unsigned long pressStartTime = 0;
const unsigned long longPressThreshold = 1000; // 1 second threshold
unsigned long currentKey = 0;
bool keyIsBeingHeld = false;

// Time tracking
unsigned long programStartTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // LED OFF (active low configuration)
  irrecv.enableIRIn();
  programStartTime = millis(); // Record program start time
}

void loop() {
  if (irrecv.decode(&results)) {
    unsigned long currentTime = millis();

    // New key press detected (non-repeat code)
    if (results.value != 0xFFFFFFFF) {
      currentKey = results.value;  // Store the actual key value
      keyIsBeingHeld = true;
      pressStartTime = currentTime;

      // Print key press with timestamp and half-minute status
      Serial.print("Key pressed: 0x");
      Serial.print(currentKey, HEX);
      Serial.print(" | Time: ");
      Serial.print(currentTime - programStartTime);
      Serial.print(" ms | 30 seconds passed: ");
      Serial.println((currentTime - programStartTime) >= 30000 ? "Yes" : "No");

      digitalWrite(LED_PIN, HIGH); // Ensure LED is off on new press
    } 
    // Repeat code detected (key held down)
    else {
      if (keyIsBeingHeld) {
        unsigned long duration = currentTime - pressStartTime;

        // Check for long press
        if (duration >= longPressThreshold) {
          // Print long press details
          Serial.print("LONG PRESS DETECTED! | Key: 0x");
          Serial.print(currentKey, HEX); // Use stored key value
          Serial.print(" | Duration: ");
          Serial.print(duration);
          Serial.print(" ms | Time since start: ");
          Serial.println(currentTime - programStartTime);

          // Activate LED (blink effect)
          digitalWrite(LED_PIN, LOW);  // Turn LED on (active low)
          delay(500);                   // Keep on for 0.5 seconds
          digitalWrite(LED_PIN, HIGH); // Turn LED off

          // Reset for next detection
          keyIsBeingHeld = false;
        }
      }
    }

    irrecv.resume(); // Receive next value
  }
}
