const int ledPins[8] = {11, 8, 7, 6, 5, 4, 3, 2}; // Reversed pin order
const int buzzerPin = 12;

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  for (int i = 0; i < 8; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // LEDs OFF initially
  }

  Serial.println("Use ONx, OFFx, TOGGLEX (e.g., ON3, TOGGLE2)");
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();  // Remove \r and spaces
    command.toUpperCase();
    processCommand(command);
  }
}

void processCommand(String cmd) {
  if ((cmd.startsWith("ON") || cmd.startsWith("OFF")) && cmd.length() == 3) {
    int led = cmd.charAt(2) - '0';
    if (led >= 0 && led <= 7) {
      digitalWrite(ledPins[led], cmd.startsWith("ON") ? HIGH : LOW);
      beep();
      Serial.println(cmd + " OK");
      return;
    }
  }

  if (cmd.startsWith("TOGGLE") && cmd.length() == 7) {
    int led = cmd.charAt(6) - '0';
    if (led >= 0 && led <= 7) {
      digitalWrite(ledPins[led], !digitalRead(ledPins[led]));
      beep();
      Serial.println("Toggled LED " + String(led));
      return;
    }
  }

  Serial.println("Invalid command: " + cmd);
}

void beep() {
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);
}
