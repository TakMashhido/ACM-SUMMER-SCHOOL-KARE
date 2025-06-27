int buzzer = 9;
int led = 13;

void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
}

void loop() {
  // SOS in Morse: ... --- ...
  playDot(); playDot(); playDot(); // 3 dots
  delay(300);
  playDash(); playDash(); playDash(); // 3 dashes
  delay(300);
  playDot(); playDot(); playDot(); // 3 dots
  delay(1000);  // Wait before repeating
}

void playDot() {
  tone(buzzer, 1000); // Start the tone for dot
  digitalWrite(led, HIGH); // LED on
  delay(200); // Duration of dot
  noTone(buzzer); // Stop tone
  digitalWrite(led, LOW); // LED off
  delay(100); // Space between signals
}

void playDash() {
  tone(buzzer, 1000); // Start the tone for dash
  digitalWrite(led, HIGH); // LED on
  delay(600); // Duration of dash
  noTone(buzzer); // Stop tone
  digitalWrite(led, LOW); // LED off
  delay(100); // Space between signals
}
