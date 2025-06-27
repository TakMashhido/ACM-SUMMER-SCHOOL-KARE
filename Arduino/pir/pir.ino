#define PIR_PIN    D5
#define GREEN_LED  D7

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Common anode: HIGH = OFF, LOW = ON
  digitalWrite(GREEN_LED, HIGH); // LED OFF at start

  Serial.begin(115200);
}

void loop() {
  bool pirState = digitalRead(PIR_PIN); // 0 = motion detected

  if (pirState == 0) { // 0 = motion!
    digitalWrite(GREEN_LED, LOW);   // ON (common anode)
    Serial.println("Motion detected! (PIR LOW)");
  } else {
    digitalWrite(GREEN_LED, HIGH);  // OFF
    Serial.println("No motion. (PIR HIGH)");
  }

  delay(200);
}
