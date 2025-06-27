const int VRx = A0;  // X-axis (up/down)
const int VRy = A1;  // Y-axis (left/right)

const int UP_LED = 4;
const int DOWN_LED = 5;
const int LEFT_LED = 6;
const int RIGHT_LED = 7;

const int THRESHOLD_HIGH = 600;
const int THRESHOLD_LOW  = 100; // Not 0—give some margin!

void setup() {
  pinMode(UP_LED, OUTPUT);
  pinMode(DOWN_LED, OUTPUT);
  pinMode(LEFT_LED, OUTPUT);
  pinMode(RIGHT_LED, OUTPUT);

  // Start with all LEDs OFF (common cathode: LOW is OFF)
  digitalWrite(UP_LED, HIGH);
  digitalWrite(DOWN_LED, HIGH);
  digitalWrite(LEFT_LED, HIGH);
  digitalWrite(RIGHT_LED, HIGH);

  Serial.begin(9600);
}

void loop() {
  int xValue = analogRead(VRx);
  int yValue = analogRead(VRy);

  Serial.print("X: ");
  Serial.print(xValue);
  Serial.print(" Y: ");
  Serial.println(yValue);

  // Turn all LEDs OFF
  digitalWrite(UP_LED, HIGH);
  digitalWrite(DOWN_LED, HIGH);
  digitalWrite(LEFT_LED, HIGH);
  digitalWrite(RIGHT_LED, HIGH);

  // Check for direction
  if (xValue > THRESHOLD_HIGH) {
    digitalWrite(UP_LED, LOW);    // UP = X high
  } else if (xValue < THRESHOLD_LOW) {
    digitalWrite(DOWN_LED, LOW);  // DOWN = X low
  } else if (yValue < THRESHOLD_LOW) {
    digitalWrite(LEFT_LED, LOW);  // LEFT = Y low
  } else if (yValue > THRESHOLD_HIGH) {
    digitalWrite(RIGHT_LED, LOW); // RIGHT = Y high
  }

  delay(50);
}
