const int ledPins[8] = {2, 3, 4, 5, 6, 7, 8, 11};
const int buzzerPin = 12;

void setup() {
  Serial.begin(9600); // HC-05 uses Serial
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  for (int i = 0; i < 8; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH); // Start with all LEDs ON
  }
}

void loop() {
  if (Serial.available()) {
    char input = Serial.read();

    if (input >= '0' && input <= '7') {
      int offLED = input - '0';

      Serial.print("Turning OFF LED: ");
      Serial.println(offLED);

      // Turn all LEDs ON first
      for (int i = 0; i < 8; i++) {
        digitalWrite(ledPins[i], HIGH);
      }

      // Turn the selected LED OFF
      digitalWrite(ledPins[offLED], LOW);

      // Beep once
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
    }
  }
}
