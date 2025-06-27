const int trigPin = 9;
const int echoPin = 10;
const int buzzerPin = 12;

const int ledPins[8] = {2, 3, 4, 5, 6, 7, 8, 11};

long duration;
int distance;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  for (int i = 0; i < 8; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
}

void loop() {
  // Trigger ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo duration and calculate distance
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Reverse LED logic: farther = more LEDs ON
  int ledsToLight = 0;
  if (distance <= 16) {
    ledsToLight = distance / 2; // 2 cm steps
    ledsToLight = constrain(ledsToLight, 0, 8);
  } else {
    ledsToLight = 8;
  }

  // Light LEDs in reverse logic
  for (int i = 0; i < 8; i++) {
    digitalWrite(ledPins[i], (i < ledsToLight) ? HIGH : LOW);
  }

  // Buzzer pattern: more LEDs OFF = more beeps
  int ledsOff = 8 - ledsToLight;

  if (ledsOff < 8) {
    for (int i = 0; i < ledsOff; i++) {
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
      delay(100);
    }
  } else {
    // Continuous beep if object is very near
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    digitalWrite(buzzerPin, LOW);
  }

  delay(200);
}
