const int LEFT_LED = 6;
const int UP_LED = 4;
const int RIGHT_LED = 7;
const int DOWN_LED = 5;
const int buzzerPin = 12;

const int ledPins[] = {LEFT_LED, UP_LED, RIGHT_LED, DOWN_LED};
const int NUM_LEDS = 4;

bool spiralMode = false;

void setup() {
  Serial.begin(9600); // HC-05 uses Serial
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH); // All LEDs OFF (common anode)
  }
}

void loop() {
  // Listen for Bluetooth command
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    Serial.print("Received: ");
    Serial.println(input);

    // Exit spiral mode if any command except "spiral"
    if (!input.equalsIgnoreCase("spiral")) {
      spiralMode = false;
      for (int i = 0; i < NUM_LEDS; i++) digitalWrite(ledPins[i], HIGH);
      digitalWrite(buzzerPin, LOW);
    }

    int ledIndex = -1;
    if (input.equalsIgnoreCase("left"))  ledIndex = 0;
    else if (input.equalsIgnoreCase("up"))    ledIndex = 1;
    else if (input.equalsIgnoreCase("right")) ledIndex = 2;
    else if (input.equalsIgnoreCase("down"))  ledIndex = 3;

    if (ledIndex != -1) {
      digitalWrite(ledPins[ledIndex], LOW); // ON for common anode, HIGH for cathode
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
    }

    // Start spiral mode
    if (input.equalsIgnoreCase("spiral")) {
      spiralMode = true;
    }
  }

  // Spiral mode animation
  while (spiralMode) {
    for (int i = 0; i < NUM_LEDS; i++) {
      // Turn all LEDs OFF
      for (int j = 0; j < NUM_LEDS; j++) digitalWrite(ledPins[j], HIGH);
      // Turn current LED ON
      digitalWrite(ledPins[i], LOW);
      delay(150);

      // If a new command comes in, exit spiral mode
      if (Serial.available()) {
        String newInput = Serial.readStringUntil('\n');
        newInput.trim();
        Serial.print("Received: ");
        Serial.println(newInput);

        if (!newInput.equalsIgnoreCase("spiral")) {
          spiralMode = false;
          for (int k = 0; k < NUM_LEDS; k++) digitalWrite(ledPins[k], HIGH);
          digitalWrite(buzzerPin, LOW);
          // Handle other commands
          int ledIndex = -1;
          if (newInput.equalsIgnoreCase("left"))  ledIndex = 0;
          else if (newInput.equalsIgnoreCase("up"))    ledIndex = 1;
          else if (newInput.equalsIgnoreCase("right")) ledIndex = 2;
          else if (newInput.equalsIgnoreCase("down"))  ledIndex = 3;
          if (ledIndex != -1) {
            digitalWrite(ledPins[ledIndex], LOW);
            digitalWrite(buzzerPin, HIGH);
            delay(100);
            digitalWrite(buzzerPin, LOW);
          }
          return; // exit the spiral animation
        }
      }
    }
  }
}
