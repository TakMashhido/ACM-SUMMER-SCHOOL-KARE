#include <DHT.h>

#define DHTPIN D4       // DHT11 data pin
#define DHTTYPE DHT11

#define RED_LED D5      // Red LED pin (GPIO14)
#define BLUE_LED D6     // Blue LED pin (GPIO12)

#define TEMP_THRESHOLD 26    // Example: turn red LED ON if temperature < 25°C
#define HUMID_THRESHOLD 37   // Example: turn blue LED ON if humidity < 50%

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  digitalWrite(RED_LED, HIGH);   // Both LEDs OFF at start
  digitalWrite(BLUE_LED, HIGH);

  Serial.println("DHT11 + LED control ready.");
}

void loop() {
  delay(2000); // 2 sec delay between readings

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT11 sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");

  // LED Logic
  if (t < TEMP_THRESHOLD) {
    digitalWrite(RED_LED, LOW);  // Red LED ON
  } else {
    digitalWrite(RED_LED, HIGH);   // Red LED OFF
  }

  if (h < HUMID_THRESHOLD) {
    digitalWrite(BLUE_LED, LOW); // Blue LED ON
  } else {
    digitalWrite(BLUE_LED, HIGH);  // Blue LED OFF
  }
}
