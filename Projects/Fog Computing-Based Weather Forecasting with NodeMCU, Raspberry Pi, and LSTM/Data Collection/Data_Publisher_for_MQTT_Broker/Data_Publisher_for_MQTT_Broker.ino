#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"

/************************* WiFi Access Point *********************************/
#define WLAN_SSID       "KLU_WIFI"
#define WLAN_PASS       ""

/************************* MQTT Broker Setup *********************************/
#define MQTT_SERVER      "10.10.42.92"
#define MQTT_SERVERPORT  1884
#define MQTT_USERNAME    "acm_iot" // Add your MQTT username 
#define MQTT_PASSWORD    "iot" // Add your MQTT password

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and all broker details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);

// Setup a 'feed' (topic) for publishing.
Adafruit_MQTT_Publish weatherFeed = Adafruit_MQTT_Publish(&mqtt, "weather/data");

/************************* DHT Sensor Setup *********************************/
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println(F("Adafruit MQTT Weather Station"));

  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  dht.begin();
}

void loop() {
  MQTT_connect();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    delay(2000);
    return;
  }

  char payload[100];
  snprintf(payload, sizeof(payload), "{\"temperature\": %.2f, \"humidity\": %.2f}", temperature, humidity);
  
  Serial.print(F("\nSending payload: "));
  Serial.println(payload);

  if (! weatherFeed.publish(payload)) {
    Serial.println(F("Failed to publish."));
  } else {
    Serial.println(F("Published!"));
  }
  
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  
  delay(5000);
}

void MQTT_connect() {
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  int8_t ret;
  while ((ret = mqtt.connect()) != 0) {
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);
  }
  Serial.println("MQTT Connected!");
}
