void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // LED OFF initially (NodeMCU: LOW = ON)
}

void loop() {
  int soundLevel = analogRead(A0);
  Serial.print("Sound level: ");
  Serial.println(soundLevel);

  if (soundLevel > 100) {
    digitalWrite(LED_BUILTIN, LOW);  // ON (active LOW for NodeMCU)
  } else {
    digitalWrite(LED_BUILTIN, HIGH); // OFF
  }
  delay(50);
}
