#define MQ2_PIN   A0
#define RED_LED   D5  // GPIO14
#define GREEN_LED D6  // GPIO12

void setup() {
  Serial.begin(115200);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
}

void loop() {
  int gasValue = analogRead(MQ2_PIN);
  Serial.print("MQ2 value: ");
  Serial.println(gasValue);

  // Adjust threshold as needed (try 300 first)
  if (gasValue > 300) {
    digitalWrite(RED_LED, HIGH);    // Smoke detected!
    digitalWrite(GREEN_LED, LOW);
  } else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);  // Normal air
  }

  delay(200);
}
