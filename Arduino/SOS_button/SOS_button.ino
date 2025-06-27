int buttonPin = 2;  // Button pin
int ledPin = 13;    // LED pin
int buzzerPin = 9;  // Buzzer pin

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  if (digitalRead(buttonPin) == HIGH) {  // Button pressed
    emergencySOS();
  }
  else {
    // Make sure everything is off if no emergency
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
  }
}

void emergencySOS() {
  // SOS in Morse code: ... --- ...
  for (int i = 0; i < 3; i++) {
    playDot();  // 3 dots
  }
  delay(300);
  for (int i = 0; i < 3; i++) {
    playDash();  // 3 dashes
  }
  delay(300);
  for (int i = 0; i < 3; i++) {
    playDot();  // 3 dots
  }
  delay(1000);  // Wait before repeating
}

void playDot() {
  tone(buzzerPin, 1000);  // Start tone for dot
  digitalWrite(ledPin, HIGH);  // LED on
  delay(200);  // Dot duration
  noTone(buzzerPin);  // Stop tone
  digitalWrite(ledPin, LOW);  // LED off
  delay(100);  // Short pause between signals
}

void playDash() {
  tone(buzzerPin, 1000);  // Start tone for dash
  digitalWrite(ledPin, HIGH);  // LED on
  delay(600);  // Dash duration
  noTone(buzzerPin);  // Stop tone
  digitalWrite(ledPin, LOW);  // LED off
  delay(100);  // Short pause between signals
}
