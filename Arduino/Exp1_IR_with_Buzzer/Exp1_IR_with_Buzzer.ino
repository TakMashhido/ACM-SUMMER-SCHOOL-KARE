// Define pin numbers
const int irSensorPin = 2;   // IR sensor output pin
const int buzzerPin = 8;     // Buzzer pin

void setup() {
  pinMode(irSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);  // Optional: for debugging
}

void loop() {
  int sensorValue = digitalRead(irSensorPin);

  if (sensorValue == HIGH) {  // HIGH means object detected for your sensor
    digitalWrite(buzzerPin, HIGH); // Turn on buzzer
    Serial.println("Object Detected!");
  } else {
    digitalWrite(buzzerPin, LOW); // Turn off buzzer
  }

  delay(1000);
}
