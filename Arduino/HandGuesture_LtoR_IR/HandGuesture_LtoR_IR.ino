const int IR_LEFT = 8;
const int IR_CENTER = 9;
const int IR_RIGHT = 10;

const int RED_LED = 11;   // For LEFT ➡️ RIGHT
const int BLUE_LED = 12;  // For RIGHT ⬅️ LEFT

unsigned long lastTriggerTime = 0;
const unsigned long gestureTimeout = 1000;

enum GestureState {
  IDLE,
  LEFT_TO_CENTER,
  LEFT_TO_RIGHT,
  RIGHT_TO_CENTER,
  RIGHT_TO_LEFT
};

GestureState gestureState = IDLE;

bool prevLeft = false;
bool prevCenter = false;
bool prevRight = false;

bool isLeftBlocked()   { return digitalRead(IR_LEFT) == LOW; }
bool isCenterBlocked() { return digitalRead(IR_CENTER) == LOW; }
bool isRightBlocked()  { return digitalRead(IR_RIGHT) == HIGH; }  // Inverted

void setup() {
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_CENTER, INPUT);
  pinMode(IR_RIGHT, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  digitalWrite(RED_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH);

  Serial.begin(9600);
  Serial.println("Gesture Detection Initialized...");
}

void loop() {
  unsigned long now = millis();

  bool left   = isLeftBlocked();
  bool center = isCenterBlocked();
  bool right  = isRightBlocked();

  bool leftEdge   = left && !prevLeft;
  bool centerEdge = center && !prevCenter;
  bool rightEdge  = right && !prevRight;

  prevLeft = left;
  prevCenter = center;
  prevRight = right;

  switch (gestureState) {
    case IDLE:
      if (leftEdge && !center && !right) {
        gestureState = LEFT_TO_CENTER;
        lastTriggerTime = now;
        Serial.println("LEFT detected");
      } else if (rightEdge && !center && !left) {
        gestureState = RIGHT_TO_CENTER;
        lastTriggerTime = now;
        Serial.println("RIGHT detected");
      }
      break;

    case LEFT_TO_CENTER:
      if (centerEdge && now - lastTriggerTime < gestureTimeout) {
        gestureState = LEFT_TO_RIGHT;
        lastTriggerTime = now;
        Serial.println("CENTER after LEFT");
      } else if (now - lastTriggerTime > gestureTimeout) {
        Serial.println("Timeout LEFT ➡️ CENTER");
        gestureState = IDLE;
      }
      break;

    case LEFT_TO_RIGHT:
      if (rightEdge && now - lastTriggerTime < gestureTimeout) {
        Serial.println("Gesture: LEFT ➡️ RIGHT ✅");
        digitalWrite(RED_LED, LOW);
        //digitalWrite(BLUE_LED, LOW);
        gestureState = IDLE;
        delay(300);
      } else if (now - lastTriggerTime > gestureTimeout) {
        Serial.println("Timeout LEFT ➡️ RIGHT");
        gestureState = IDLE;
      }
      break;

    case RIGHT_TO_CENTER:
      if (centerEdge && now - lastTriggerTime < gestureTimeout) {
        gestureState = RIGHT_TO_LEFT;
        lastTriggerTime = now;
        Serial.println("CENTER after RIGHT");
      } else if (now - lastTriggerTime > gestureTimeout) {
        Serial.println("Timeout RIGHT ➡️ CENTER");
        gestureState = IDLE;
      }
      break;

    case RIGHT_TO_LEFT:
      if (leftEdge && now - lastTriggerTime < gestureTimeout) {
        Serial.println("Gesture: RIGHT ⬅️ LEFT ✅");
        //digitalWrite(RED_LED, LOW);
        digitalWrite(BLUE_LED, LOW);
        gestureState = IDLE;
        delay(300);
      } else if (now - lastTriggerTime > gestureTimeout) {
        Serial.println("Timeout RIGHT ➡️ LEFT");
        gestureState = IDLE;
      }
      break;
  }

  delay(50);  // smooth polling
}
