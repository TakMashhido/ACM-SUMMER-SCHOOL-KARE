const int LDR_PIN = A0;      // LDR analog pin
const int RELAY1 = 7;        // Relay 1
const int RELAY2 = 6;        // Relay 2
const int RELAY3 = 5;        // Relay 3
const int RELAY4 = 4;        // Relay 4

const int threshold = 50;   // Adjust this value for your darkness level (0=dark, 1023=bright)

void setup() {
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  // Start with all relays OFF (HIGH for most modules, LOW for some)
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, HIGH);
  digitalWrite(RELAY3, HIGH);
  digitalWrite(RELAY4, HIGH);

  Serial.begin(9600);
}

void loop() {
  int ldrValue = analogRead(LDR_PIN);
  Serial.print("LDR value: ");
  Serial.println(ldrValue);

  if (ldrValue < threshold) {
    // It's dark, turn ON all relays
    digitalWrite(RELAY1, HIGH); // LOW = ON for most relay boards
    digitalWrite(RELAY2, LOW);
    digitalWrite(RELAY3, LOW);  
    digitalWrite(RELAY4, LOW);
  } else {
    // It's bright, turn OFF all relays
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, HIGH);
  }
  delay(200);
}
