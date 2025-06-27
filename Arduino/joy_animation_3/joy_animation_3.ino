const int UP_LED = 4;
const int DOWN_LED = 5;
const int LEFT_LED = 6;
const int RIGHT_LED = 7;
const int VRx = A0;
const int VRy = A1;

const int THRESHOLD_HIGH = 800;
const int THRESHOLD_LOW  = 200;
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

void spiralStep(int step) {
  int leds[4] = {UP_LED, RIGHT_LED, DOWN_LED, LEFT_LED};
  for (int i = 0; i < 4; i++) digitalWrite(leds[i], HIGH);
  digitalWrite(leds[step % 4], LOW);
}

void waveStep(int step) {
  int leds[4] = {LEFT_LED, UP_LED, RIGHT_LED, DOWN_LED};
  int index;
  if (step < 4) {
    index = step;
  } else {
    index = 6 - step; // counts backward: 3,2,1,0
  }
  for (int i = 0; i < 4; i++) digitalWrite(leds[i], HIGH);
  digitalWrite(leds[index], LOW);
}

void loop() {
  int xValue = analogRead(VRx);
  int yValue = analogRead(VRy);

  // --- Spiral while holding UP-RIGHT ---
  if (xValue > THRESHOLD_HIGH && yValue > THRESHOLD_HIGH) {
    Serial.println("Continuous Spiral!");
    int step = 0;
    while (analogRead(VRx) > THRESHOLD_HIGH && analogRead(VRy) > THRESHOLD_HIGH) {
      spiralStep(step);
      step = (step + 1) % 4;
      delay(DELAY_MS);
    }
    // Turn all off at release
    digitalWrite(UP_LED, HIGH);
    digitalWrite(DOWN_LED, HIGH);
    digitalWrite(LEFT_LED, HIGH);
    digitalWrite(RIGHT_LED, HIGH);
  }

  // --- Wave while holding DOWN-LEFT ---
  else if (xValue < THRESHOLD_LOW && yValue < THRESHOLD_LOW) {
    Serial.println("Continuous Wave!");
    int step = 0;
    while (analogRead(VRx) < THRESHOLD_LOW && analogRead(VRy) < THRESHOLD_LOW) {
      waveStep(step);
      step = (step + 1) % 7; // 0-3 forward, 4-6 backward
      delay(DELAY_MS);
    }
    // Turn all off at release
    digitalWrite(UP_LED, HIGH);
    digitalWrite(DOWN_LED, HIGH);
    digitalWrite(LEFT_LED, HIGH);
    digitalWrite(RIGHT_LED, HIGH);
  }

  // Otherwise, all LEDs off
  else {
    digitalWrite(UP_LED, HIGH);
    digitalWrite(DOWN_LED, HIGH);
    digitalWrite(LEFT_LED, HIGH);
    digitalWrite(RIGHT_LED, HIGH);
  }
}
