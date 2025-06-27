#define FLAME_DO  D5
#define RED_LED   D7
#define GREEN_LED D8

void setup() {
  Serial.begin(115200);
  pinMode(FLAME_DO, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  digitalWrite(RED_LED, HIGH);   // OFF (common anode)
  digitalWrite(GREEN_LED, LOW);  // ON  (common anode)
}

void loop() {
  int flame = digitalRead(FLAME_DO);

  Serial.print("Flame sensor value: ");
  Serial.println(flame);

  if (flame == 0) { // 0 = Flame detected!
    digitalWrite(RED_LED, LOW);     // ON  (common anode)
    digitalWrite(GREEN_LED, HIGH);  // OFF (common anode)
    Serial.println("🔥 FLAME DETECTED!");
  } else {
    digitalWrite(RED_LED, HIGH);    // OFF (common anode)
    digitalWrite(GREEN_LED, LOW);   // ON  (common anode)
    Serial.println("✅ No flame.");
  }

  delay(200);
}
