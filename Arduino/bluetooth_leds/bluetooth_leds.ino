const int LEFT_LED = 6;   // Change as per your wiring
const int UP_LED = 4;
const int RIGHT_LED = 7;
const int DOWN_LED = 5;
const int buzzerPin = 12;

// Helper: All LEDs in array for easier off/on
const int ledPins[] = {LEFT_LED, UP_LED, RIGHT_LED, DOWN_LED};
const int NUM_LEDS = 4;

void setup() {
  Serial.begin(9600); // HC-05 default
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH); // Start with all LEDs OFF (common anode)
  }
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n'); // Read until newline
    input.trim(); // Remove any whitespace/newlines

    Serial.print("Received: ");
    Serial.println(input);

    // First, turn all LEDs OFF
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(ledPins[i], HIGH); // OFF for common anode, LOW for cathode
    }

    // Determine which LED to turn ON
    int ledIndex = -1;
    if (input.equalsIgnoreCase("left"))  ledIndex = 0;
    else if (input.equalsIgnoreCase("up"))    ledIndex = 1;
    else if (input.equalsIgnoreCase("right")) ledIndex = 2;
    else if (input.equalsIgnoreCase("down"))  ledIndex = 3;

    if (ledIndex != -1) {
      digitalWrite(ledPins[ledIndex], LOW); // ON for common anode, HIGH for cathode

      // Optional: Beep for feedback
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
    }
  }
}
