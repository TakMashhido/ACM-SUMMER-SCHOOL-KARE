const int UP_LED = 4;
const int DOWN_LED = 5;
const int LEFT_LED = 6;
const int RIGHT_LED = 7;
const int VRx = A0;
const int VRy = A1;
const int SW_PIN = 2;

const int THRESHOLD_HIGH = 800;
const int THRESHOLD_LOW  = 200;
const int DELAY_MS = 100;

bool swapped = false; // false = normal, true = swapped
bool prevSwitch = HIGH;

void setup() {
  pinMode(UP_LED, OUTPUT);
  pinMode(DOWN_LED, OUTPUT);
  pinMode(LEFT_LED, OUTPUT);
  pinMode(RIGHT_LED, OUTPUT);
  pinMode(SW_PIN, INPUT_PULLUP); // Joystick SW is usually active LOW

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
    index = 6 - step;
  }
  for (int i = 0; i < 4; i++) digitalWrite(leds[i], HIGH);
  digitalWrite(leds[index], LOW);
}

void loop() {
  int xValue = analogRead(VRx);
  int yValue = analogRead(VRy);
  bool switchState = digitalRead(SW_PIN); // LOW when pressed

  // Toggle swapped mode when button is pressed and released
  if (prevSwitch == HIGH && switchState == LOW) {
    swapped = !swapped;
    Serial.print("Animation modes ");
    Serial.println(swapped ? "swapped!" : "normal!");
    delay(200); // debounce
  }
  prevSwitch = switchState;

  // Animation logic
  if (!swapped) {
    // Normal: UP-RIGHT = spiral, DOWN-LEFT = wave
    if (xValue > THRESHOLD_HIGH && yValue > THRESHOLD_HIGH) {
      int step = 0;
      while (analogRead(VRx) > THRESHOLD_HIGH && analogRead(VRy) > THRESHOLD_HIGH &&
             digitalRead(SW_PIN) == switchState) {
        spiralStep(step);
        step = (step + 1) % 4;
        delay(DELAY_MS);
      }
    } else if (xValue < THRESHOLD_LOW && yValue < THRESHOLD_LOW) {
      int step = 0;
      while (analogRead(VRx) < THRESHOLD_LOW && analogRead(VRy) < THRESHOLD_LOW &&
             digitalRead(SW_PIN) == switchState) {
        waveStep(step);
        step = (step + 1) % 7;
        delay(DELAY_MS);
      }
    }
  } else {
    // Swapped: UP-RIGHT = wave, DOWN-LEFT = spiral
    if (xValue > THRESHOLD_HIGH && yValue > THRESHOLD_HIGH) {
      int step = 0;
      while (analogRead(VRx) > THRESHOLD_HIGH && analogRead(VRy) > THRESHOLD_HIGH &&
             digitalRead(SW_PIN) == switchState) {
        waveStep(step);
        step = (step + 1) % 7;
        delay(DELAY_MS);
      }
    } else if (xValue < THRESHOLD_LOW && yValue < THRESHOLD_LOW) {
      int step = 0;
      while (analogRead(VRx) < THRESHOLD_LOW && analogRead(VRy) < THRESHOLD_LOW &&
             digitalRead(SW_PIN) == switchState) {
        spiralStep(step);
        step = (step + 1) % 4;
        delay(DELAY_MS);
      }
    }
  }

  // Always turn all LEDs off if no animation running
  digitalWrite(UP_LED, HIGH);
  digitalWrite(DOWN_LED, HIGH);
  digitalWrite(LEFT_LED, HIGH);
  digitalWrite(RIGHT_LED, HIGH);
}
