#define PIR_PIN   D5
#define IR_PIN    D6
#define GREEN_LED D7
#define RED_LED   D8

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  // Start with all LEDs OFF (common anode)
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, HIGH);

  Serial.begin(115200);
}

void loop() {
  bool pir = digitalRead(PIR_PIN); // 0 = motion (human/animal), 1 = no motion
  bool ir  = digitalRead(IR_PIN);  // 1 = object detected

  if ((pir == 0) && (ir == 1)) {
    // Human or animal
    digitalWrite(GREEN_LED, LOW);   // ON (common anode)
    digitalWrite(RED_LED, HIGH);    // OFF
    Serial.println("Human/animal detected: GREEN LED ON");
    // Here you can also trigger camera/buzzer if desired!
  } else if ((pir == 1) && (ir == 1)) {
    // Robot/object only
    digitalWrite(GREEN_LED, HIGH);  // OFF
    digitalWrite(RED_LED, LOW);     // ON
    Serial.println("Object/robot detected: RED LED ON");
    // You could trigger robot action here if you want!
  } else {
    // Nothing detected
    digitalWrite(GREEN_LED, HIGH);  // OFF
    digitalWrite(RED_LED, HIGH);    // OFF
    Serial.println("No detection: All LEDs OFF");
  }

  delay(200);
}
