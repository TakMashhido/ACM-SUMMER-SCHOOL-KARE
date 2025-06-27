#include <IRremote.h>

// Pin definitions
#define IR_RECEIVE_PIN 7   // IR receiver connected to pin 7
#define LED_PIN 13         // LED connected to pin 13

// Define the IR code for your toggle button (replace with your actual code)
#define TOGGLE_BUTTON 0xBF40FE01

bool ledState = false;
bool buttonWasPressed = false;

IRrecv irrecv(IR_RECEIVE_PIN);

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  irrecv.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

void loop() {
  if (irrecv.decode()) {
    unsigned long codeValue = irrecv.decodedIRData.decodedRawData;

    // Ignore invalid and repeat codes
    if (codeValue == 0xFFFFFFFF || codeValue == 0x0) {
      irrecv.resume();
      return;
    }

    Serial.print("IR Code: 0x");
    Serial.println(codeValue, HEX);

    // Toggle only on a new press
    if (codeValue == TOGGLE_BUTTON && !buttonWasPressed) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
      Serial.println(ledState ? "LED ON" : "LED OFF");
      buttonWasPressed = true;
    }
    // Only reset the buttonWasPressed flag if a **different** valid code comes in
    if (codeValue != TOGGLE_BUTTON) {
      buttonWasPressed = false;
    }

    irrecv.resume();
  }
}
