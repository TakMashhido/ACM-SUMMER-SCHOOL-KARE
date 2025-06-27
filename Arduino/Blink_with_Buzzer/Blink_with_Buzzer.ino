/*
  ESP8266 Blink with Beep Sound
  Blink the blue LED on the ESP-01 module and generate a beep sound using a piezo buzzer.
  This example code is in the public domain

  The blue LED on the ESP-01 module is connected to GPIO1
  (which is also the TXD pin; so we cannot use Serial.print() at the same time)

  Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/

// Define the pin for the piezo buzzer
#define BUZZER_PIN 2

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // Initialize the LED_BUILTIN pin as an output
  pinMode(BUZZER_PIN, OUTPUT);   // Initialize the buzzer pin as an output
}

// The loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, LOW);  // Turn the LED on (active low)
  tone(BUZZER_PIN, 1000);          // Generate a 1kHz beep sound on the buzzer
  delay(1000);                     // Wait for a second

  digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off
  noTone(BUZZER_PIN);              // Stop the beep sound
  delay(2000);                     // Wait for two seconds
}
