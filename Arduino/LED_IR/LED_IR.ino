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
unsigned long lastLongPressedKey = 0;
bool keyIsBeingHeld = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  irrecv.enableIRIn();
}

void loop() {
  if (irrecv.decode(&results)) {
    unsigned long currentTime = millis();
    
    // New key press detected (non-repeat code)
    if (results.value != 0xFFFFFFFF) {
      currentKey = results.value;  // Store the actual key value
      keyIsBeingHeld = true;
      pressStartTime = currentTime;
      Serial.print("Key pressed: 0x");
      Serial.println(currentKey, HEX);
      digitalWrite(LED_PIN, LOW);  // Reset LED on new press
    } 
    // Repeat code detected (key held down)
    else {
      if (keyIsBeingHeld && (currentTime - pressStartTime >= longPressThreshold)) {
        lastLongPressedKey = currentKey;  // Use stored key value
        Serial.print("Long press detected. Stored key: 0x");
        Serial.println(lastLongPressedKey, HEX);
        
        // Perform action with the stored key
        digitalWrite(LED_PIN, HIGH);
        
        // Reset for next detection
        keyIsBeingHeld = false;
      }
    }
    
    irrecv.resume(); // Receive next value
  }
}
