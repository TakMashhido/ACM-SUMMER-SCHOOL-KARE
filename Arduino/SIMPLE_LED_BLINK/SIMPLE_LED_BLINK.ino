void setup() {
  pinMode(D2, OUTPUT);  // Use D2 (GPIO4) for external LED
}

void loop() {
  digitalWrite(D2, LOW);    // Turn LED on (if wired active low)
  delay(1000);
  digitalWrite(D2, HIGH);   // Turn LED off
  delay(1000);
}
