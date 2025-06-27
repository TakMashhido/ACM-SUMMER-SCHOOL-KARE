const int UP_LED = 4;
const int DOWN_LED = 5;
const int LEFT_LED = 6;
const int RIGHT_LED = 7;
const int VRx = A0;
const int THRESHOLD_HIGH = 600;
const int DELAY_MS = 100;

void setup() {
  pinMode(UP_LED, OUTPUT);
  pinMode(DOWN_LED, OUTPUT);
  pinMode(LEFT_LED, OUTPUT);
  pinMode(RIGHT_LED, OUTPUT);
  digitalWrite(UP_LED, HIGH);
  digitalWrite(DOWN_LED, HIGH);
  digitalWrite(LEFT_LED, HIGH);
  digitalWrite(RIGHT_LED, HIGH);
  Serial.begin(9600);
}

void knightRider() {
  int leds[4] = {UP_LED, RIGHT_LED, DOWN_LED, LEFT_LED};
  // Forward
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) digitalWrite(leds[j], HIGH);
    digitalWrite(leds[i], LOW);
    delay(DELAY_MS);
  }
  // Backward
  for (int i = 2; i > 0; i--) {
    for (int j = 0; j < 4; j++) digitalWrite(leds[j], HIGH);
    digitalWrite(leds[i], LOW);
    delay(DELAY_MS);
  }
  // All off at the end
  for (int j = 0; j < 4; j++) digitalWrite(leds[j], HIGH);
}

void loop() {
  int xValue = analogRead(VRx);
  if (xValue > THRESHOLD_HIGH) {
    Serial.println("UP detected - Running LED animation!");
    knightRider();
    delay(300);
  }
}
