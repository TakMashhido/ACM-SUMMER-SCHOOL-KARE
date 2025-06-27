const int UP_LED = 4;
const int DOWN_LED = 5;
const int LEFT_LED = 6;
const int RIGHT_LED = 7;
const int VRx = A0;
const int VRy = A1;
const int THRESHOLD = 600;
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

void spiral(int rounds) {
  int leds[4] = {UP_LED, RIGHT_LED, DOWN_LED, LEFT_LED};
  for (int r = 0; r < rounds; r++) {
    for (int i = 0; i < 4; i++) {
      // Turn all off
      for (int j = 0; j < 4; j++) digitalWrite(leds[j], HIGH);
      // Turn on one
      digitalWrite(leds[i], LOW);
      delay(DELAY_MS);
    }
  }
  // All off at end
  for (int j = 0; j < 4; j++) digitalWrite(leds[j], HIGH);
}

void loop() {
  int xValue = analogRead(VRx);
  int yValue = analogRead(VRy);

  // Example: Trigger spiral when joystick is up-right (both high)
  if (xValue > THRESHOLD && yValue > THRESHOLD) {
    Serial.println("Spiral animation!");
    spiral(3);  // 3 spiral loops
    delay(300);
  }
}
