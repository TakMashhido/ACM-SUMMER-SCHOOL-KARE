const int SW_PIN = 2;              // Joystick switch pin

// LED pins in your desired order
const int LEDS[] = {6, 4, 7, 5};   // Left, Up, Right, Down
const int NUM_LEDS = 4;

bool prevSwitchState = HIGH;        // With INPUT_PULLUP, HIGH = not pressed

void setup() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LEDS[i], OUTPUT);
    digitalWrite(LEDS[i], HIGH); // All OFF at start (HIGH = OFF for common anode)
  }
  pinMode(SW_PIN, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  bool switchState = digitalRead(SW_PIN);

  // Button just pressed: Go!
  if (prevSwitchState == HIGH && switchState == LOW) {
    Serial.println("Go!");
    // Turn LEDs ON: Left -> Up -> Right -> Down
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(LEDS[i], LOW); // ON for common anode, HIGH for cathode
      delay(150);
    }
  }

  // Button just released: Come back!
  if (prevSwitchState == LOW && switchState == HIGH) {
    Serial.println("Come back!");
    // Turn LEDs OFF: Down -> Right -> Up -> Left
    for (int i = NUM_LEDS - 1; i >= 0; i--) {
      digitalWrite(LEDS[i], HIGH); // OFF for common anode, LOW for cathode
      delay(150);
    }
  }

  prevSwitchState = switchState;
  delay(10);
}
