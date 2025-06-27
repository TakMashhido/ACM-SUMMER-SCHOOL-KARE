#include <IRremote.h>

// Pin definitions
#define IR_RECEIVE_PIN 7   // IR receiver connected to pin 7 (update as per your wiring)
#define LED_PIN 13         // LED connected to pin 13 (built-in LED)

IRrecv irrecv(IR_RECEIVE_PIN);

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // LED off initially
  irrecv.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Enable IR receiver with LED feedback
}

void loop() {
  if (irrecv.decode()) { // Check if IR signal is received
    unsigned long value = irrecv.decodedIRData.decodedRawData;
    Serial.print("IR Code: 0x");
    Serial.println(value, HEX);  // Print IR code to serial monitor

    // Turn on LED when a button is pressed
    digitalWrite(LED_PIN, HIGH); // LED ON

    delay(500); // LED stays on for 0.5 seconds

    // Turn off LED
    digitalWrite(LED_PIN, LOW);  // LED OFF

    irrecv.resume(); // Prepare to receive next IR signal
  }
}
