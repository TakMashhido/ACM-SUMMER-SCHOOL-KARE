#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

#define DHTPIN D3        // DHT11 data pin
#define DHTTYPE DHT11

#define BUILTIN_LED LED_BUILTIN
#define BLUE_LED D5      // (optional)

#define TEMP_THRESHOLD 30
#define HUMID_THRESHOLD 30

const char* ssid = "RajuPhone";
const char* password = "AkkiAshu3020";
unsigned long myChannelNumber = 2995157; // e.g., 123456
const char *myWriteAPIKey = "95SDSCXP9N8YD25A";

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  digitalWrite(BLUE_LED, LOW);

  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  ThingSpeak.begin(client);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT11!");
    delay(2000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C\tHumidity: ");
  Serial.print(h);
  Serial.println(" %");

  // LED logic
  digitalWrite(BUILTIN_LED, t < TEMP_THRESHOLD ? LOW : HIGH);
  digitalWrite(BLUE_LED, h < HUMID_THRESHOLD ? HIGH : LOW);

  // Upload to ThingSpeak
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.print("Problem updating channel. HTTP error code ");
    Serial.println(x);
  }

  delay(20000); // ThingSpeak allows updates every 15 seconds (min), 20s is safe
}
