const int ledPins[8] = {2, 3, 4, 5, 6, 7, 8, 11}; // LED pins
const int buzzerPin = 12;

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  for (int i = 0; i < 8; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // All LEDs OFF
  }

  Serial.println("Say ON 3 or OFF 2 etc.");
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();                         // Remove leading/trailing spaces
    command.replace(" ", "");               // Remove all spaces inside
    command.toUpperCase();                  // Convert to uppercase
    processCommand(command);
  }
}

void processCommand(String cmd) {
  if ((cmd.startsWith("ON") || cmd.startsWith("OFF")) && cmd.length() == 3) {
    char ch = cmd.charAt(2);
    if (ch >= '0' && ch <= '7') {
      int led = ch - '0';
      bool turnOn = cmd.startsWith("ON");
      digitalWrite(ledPins[led], turnOn ? HIGH : LOW);
      beep();
      Serial.println((turnOn ? "ON" : "OFF") + String(led) + " OK");
    }
  } else {
    Serial.println("Invalid command: " + cmd);
  }
}

void beep() {
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);
}
