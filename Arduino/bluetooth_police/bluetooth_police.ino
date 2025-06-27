const int LEFT_LED = 6;   // Change as per your wiring
const int UP_LED = 4;
const int RIGHT_LED = 7;
const int DOWN_LED = 5;
const int buzzerPin = 8;

const int ledPins[] = {LEFT_LED, UP_LED, RIGHT_LED, DOWN_LED};
const int NUM_LEDS = 4;

bool policeMode = false;

void setup() {
  Serial.begin(9600); // HC-05 default
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH); // All LEDs OFF (common anode)
  }
}

void loop() {
  // Check for Bluetooth command
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    Serial.print("Received: ");
    Serial.println(input);

    // Turn OFF police mode if any command except "police"
    if (!input.equalsIgnoreCase("police")) {
      policeMode = false;
      // Turn all LEDs OFF
      for (int i = 0; i < NUM_LEDS; i++) {
        digitalWrite(ledPins[i], HIGH);
      }
    }

    int ledIndex = -1;
    if (input.equalsIgnoreCase("left"))  ledIndex = 0;
    else if (input.equalsIgnoreCase("up"))    ledIndex = 1;
    else if (input.equalsIgnoreCase("right")) ledIndex = 2;
    else if (input.equalsIgnoreCase("down"))  ledIndex = 3;

    if (ledIndex != -1) {
      digitalWrite(ledPins[ledIndex], LOW); // ON for common anode, HIGH for cathode
      // Beep for feedback
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
    }

    // Start police mode
    if (input.equalsIgnoreCase("police")) {
      policeMode = true;
    }
  }

  // Police mode effect (LEFT and RIGHT flash, buzzer beeps)
  while (policeMode) {
    // LEFT ON, RIGHT OFF
    digitalWrite(LEFT_LED, LOW);
    digitalWrite(RIGHT_LED, HIGH);
    digitalWrite(UP_LED, HIGH);
    digitalWrite(DOWN_LED, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(150);

    // LEFT OFF, RIGHT ON
    digitalWrite(LEFT_LED, HIGH);
    digitalWrite(RIGHT_LED, LOW);
    digitalWrite(buzzerPin, LOW);
    delay(150);

    // Check if a new command comes in to exit police mode
    if (Serial.available()) {
      String newInput = Serial.readStringUntil('\n');
      newInput.trim();
      Serial.print("Received: ");
      Serial.println(newInput);

      if (!newInput.equalsIgnoreCase("police")) {
        policeMode = false;
        // Turn all LEDs OFF
        for (int i = 0; i < NUM_LEDS; i++) {
          digitalWrite(ledPins[i], HIGH);
        }
        digitalWrite(buzzerPin, LOW);
        // Optionally, handle other commands immediately:
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
        break;
      }
    }
  }
}
