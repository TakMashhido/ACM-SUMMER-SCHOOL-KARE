const int irSensorPin = 2;
const int buzzerPin = 8;
bool objectPreviouslyDetected = false;
int detectionCount = 0;

void setup() {
  pinMode(irSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int sensorValue = digitalRead(irSensorPin);

  if (sensorValue == HIGH && !objectPreviouslyDetected) {
    // New object detected
    digitalWrite(buzzerPin, HIGH);
    Serial.println("Object Detected!");

    detectionCount++;
    Serial.print("Total Detections: ");
    Serial.println(detectionCount);

    delay(500); // Buzz for 0.5 seconds
    digitalWrite(buzzerPin, LOW);

    objectPreviouslyDetected = true;
  } 
  else if (sensorValue == LOW) {
    // No object in range
    objectPreviouslyDetected = false;
  }

  delay(100);
}
