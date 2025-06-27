#define MQ2_PIN   A0     // Smoke sensor analog
#define FLAME_DO  D5     // Flame sensor digital out
#define BUZZER    D6
#define RED_LED   D7
#define GREEN_LED D8

void setup() {
  Serial.begin(115200);
  pinMode(FLAME_DO, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  digitalWrite(BUZZER, LOW);
  digitalWrite(RED_LED, HIGH);   // OFF (common anode)
  digitalWrite(GREEN_LED, LOW);  // ON  (common anode)
}

void loop() {
  int smokeValue = analogRead(MQ2_PIN);
  int flame = digitalRead(FLAME_DO);

  Serial.print("Smoke: ");
  Serial.print(smokeValue);
  Serial.print("  Flame: ");
  Serial.println(flame);

  bool danger = false;

  if (smokeValue > 700) {  // TUNE threshold!
    Serial.println("🚨 Smoke detected!");
    danger = true;
  }
  if (flame == 0) {  // 0 = Flame detected (per your sensor)
    Serial.println("🔥 Flame detected!");
    danger = true;
  }

  if (danger) {
    digitalWrite(RED_LED, LOW);     // ON  (common anode)
    digitalWrite(GREEN_LED, HIGH);  // OFF (common anode)
    digitalWrite(BUZZER, HIGH);     // ON
    Serial.println("!!! SOS: FIRE/SMOKE EMERGENCY !!!");
    blinkSOS();  // Optional: blink "SOS" in Morse code
  } else {
    digitalWrite(RED_LED, HIGH);    // OFF (common anode)
    digitalWrite(GREEN_LED, LOW);   // ON  (common anode)
    digitalWrite(BUZZER, LOW);      // OFF
  }

  delay(500);
}

// Optional: Morse code "SOS" blink on Red LED
void blinkSOS() {
  // S: ...
  for (int i=0; i<3; i++) { blinkDot(); }
  delay(500);
  // O: ---
  for (int i=0; i<3; i++) { blinkDash(); }
  delay(500);
  // S: ...
  for (int i=0; i<3; i++) { blinkDot(); }
  delay(1000);
}
void blinkDot() {
  digitalWrite(RED_LED, LOW); delay(100);   // ON
  digitalWrite(RED_LED, HIGH); delay(100);  // OFF
}
void blinkDash() {
  digitalWrite(RED_LED, LOW); delay(400);   // ON
  digitalWrite(RED_LED, HIGH); delay(100);  // OFF
}
