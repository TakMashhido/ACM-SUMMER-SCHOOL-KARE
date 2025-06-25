// --- Libraries ---
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "DHT.h"
#include <math.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// =================================================================
// ACTION: CONFIGURE YOUR SETTINGS HERE
// =================================================================

// --- WiFi Configuration ---
const char* WIFI_SSID = "8501A - 2";
const char* WIFI_PASSWORD = "";

// --- MQTT Broker Configuration ---
const char* MQTT_BROKER_HOST = "10.10.42.92"; // Or hostname
const int   MQTT_BROKER_PORT = 1884;
const char* MQTT_USERNAME = "acm_iot"; // Your MQTT username
const char* MQTT_PASSWORD = "iot"; // Your MQTT password

// --- MQTT Topics ---
// Base topic for all publications.
#define MQTT_BASE_TOPIC "pi/weather"

// --- Location Configuration ---
// Set your location's altitude in meters for accurate Sea Level Pressure.
// An estimate for the Krishnan Kovil area is used here.
#define ALTITUDE_METERS 140.0

// =================================================================
// SENSOR & CLIENT OBJECTS
// =================================================================

// --- Sensor Objects ---
Adafruit_BMP280 bmp;
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- WiFi & MQTT Client Objects (Adafruit MQTT Version) ---
WiFiClient espClient;
Adafruit_MQTT_Client mqtt(&espClient, MQTT_BROKER_HOST, MQTT_BROKER_PORT, MQTT_USERNAME, MQTT_PASSWORD);

// --- MQTT Publisher Objects ---
// Create an Adafruit_MQTT_Publish object for each piece of data you want to send.
Adafruit_MQTT_Publish pub_temp_dht = Adafruit_MQTT_Publish(&mqtt, MQTT_BASE_TOPIC "/primary/temperature_dht");
Adafruit_MQTT_Publish pub_temp_bmp = Adafruit_MQTT_Publish(&mqtt, MQTT_BASE_TOPIC "/primary/temperature_bmp");
Adafruit_MQTT_Publish pub_humidity = Adafruit_MQTT_Publish(&mqtt, MQTT_BASE_TOPIC "/primary/humidity");
Adafruit_MQTT_Publish pub_pressure = Adafruit_MQTT_Publish(&mqtt, MQTT_BASE_TOPIC "/primary/pressure");
Adafruit_MQTT_Publish pub_heat_index = Adafruit_MQTT_Publish(&mqtt, MQTT_BASE_TOPIC "/calculated/heat_index");
Adafruit_MQTT_Publish pub_dew_point = Adafruit_MQTT_Publish(&mqtt, MQTT_BASE_TOPIC "/calculated/dew_point");
Adafruit_MQTT_Publish pub_abs_humidity = Adafruit_MQTT_Publish(&mqtt, MQTT_BASE_TOPIC "/calculated/absolute_humidity");
Adafruit_MQTT_Publish pub_vpd = Adafruit_MQTT_Publish(&mqtt, MQTT_BASE_TOPIC "/calculated/vpd");
Adafruit_MQTT_Publish pub_air_density = Adafruit_MQTT_Publish(&mqtt, MQTT_BASE_TOPIC "/calculated/air_density");
Adafruit_MQTT_Publish pub_sea_level_pressure = Adafruit_MQTT_Publish(&mqtt, MQTT_BASE_TOPIC "/calculated/sea_level_pressure");


// --- Global variables to hold all sensor & calculated data ---
float humidity = 0.0, tempC_dht = 0.0, tempC_bmp = 0.0;
float station_pressure_pa = 0.0, heatIndex = 0.0, dewPoint = 0.0;
float absoluteHumidity = 0.0, vpd = 0.0, airDensity = 0.0, seaLevelPressure = 0.0;

// =================================================================
// SETUP: Runs once on boot
// =================================================================
void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Weather Station - Booting up...");

  initSensors();
  setup_wifi();
  // MQTT Client is configured in its constructor, no setServer needed.
}

// =================================================================
// LOOP: Runs continuously
// =================================================================
void loop() {
  // Ensure we are connected to the MQTT broker
  MQTT_connect();

  // The Adafruit library uses a different method for keeping the connection alive
  // and processing messages. This is now handled within the MQTT_connect function
  // and during publish calls, so a constant mqtt.loop() isn't strictly necessary
  // unless you are also subscribing to topics.

  // Step 1: Read sensors and perform all calculations
  updateSensorData();

  // Step 2: Print all the data to the Serial Monitor (for debugging)
  printDataToSerial();

  // Step 3: Publish all the data to MQTT
  publishMqttData();

  // Step 4: Wait for 5 seconds before the next reading
  delay(5000);
}

// =================================================================
// INITIALIZATION FUNCTIONS
// =================================================================

void initSensors() {
  // Initialize BMP280 Sensor
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BMP280 sensor! Check wiring or I2C address.");
    while (1) delay(10);
  }

  // Initialize DHT11 Sensor
  dht.begin();
  Serial.println("Sensors Initialized.");
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// =================================================================
// MQTT FUNCTIONS (Adafruit MQTT Version)
// =================================================================

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for success
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

// Helper function to publish a float value. Returns true on success.
bool publishFloat(Adafruit_MQTT_Publish &publisher, float value) {
  char payload[10];
  // Convert float to string (4 width, 2 precision) for publishing
  dtostrf(value, 4, 2, payload);
  return publisher.publish(payload);
}

void publishMqttData() {
  Serial.println("\nPublishing data to MQTT Broker...");

  // This is a C++ preprocessor macro. It helps reduce repetitive code.
  // It creates a block of code for logging and publishing each value.
  #define MQTT_PUBLISH(pub_object, value, topic_suffix) \
    Serial.print("  -> " MQTT_BASE_TOPIC topic_suffix "..."); \
    if (publishFloat(pub_object, value)) { \
      Serial.println(" OK!"); \
    } else { \
      Serial.println(" FAILED!"); \
    }

  // Publish Primary Sensor Readings using the macro
  MQTT_PUBLISH(pub_temp_dht, tempC_dht, "/primary/temperature_dht");
  MQTT_PUBLISH(pub_temp_bmp, tempC_bmp, "/primary/temperature_bmp");
  MQTT_PUBLISH(pub_humidity, humidity, "/primary/humidity");
  MQTT_PUBLISH(pub_pressure, station_pressure_pa / 100.0, "/primary/pressure");

  // Publish Calculated Values using the macro
  MQTT_PUBLISH(pub_heat_index, heatIndex, "/calculated/heat_index");
  MQTT_PUBLISH(pub_dew_point, dewPoint, "/calculated/dew_point");
  MQTT_PUBLISH(pub_abs_humidity, absoluteHumidity, "/calculated/absolute_humidity");
  MQTT_PUBLISH(pub_vpd, vpd, "/calculated/vpd");
  MQTT_PUBLISH(pub_air_density, airDensity, "/calculated/air_density");
  MQTT_PUBLISH(pub_sea_level_pressure, seaLevelPressure, "/calculated/sea_level_pressure");

  Serial.println("MQTT Publishing Complete.");

  // Undefine the macro to keep its scope local to this function
  #undef MQTT_PUBLISH
}


// =================================================================
// SENSOR & DATA FUNCTIONS (from original code)
// =================================================================

void updateSensorData() {
  float newHum = dht.readHumidity();
  float newTemp = dht.readTemperature();

  if (isnan(newHum) || isnan(newTemp)) {
    Serial.println("Failed to read from DHT sensor! Retaining old values.");
    return;
  }

  humidity = newHum;
  tempC_dht = newTemp;
  tempC_bmp = bmp.readTemperature();
  station_pressure_pa = bmp.readPressure();

  // Calculations
  double saturationVaporPressure = 611.2 * exp((17.67 * tempC_dht) / (tempC_dht + 243.5));
  double actualVaporPressure = saturationVaporPressure * (humidity / 100.0);

  heatIndex = dht.computeHeatIndex(tempC_dht, humidity, false);
  dewPoint = 243.04 * (log(humidity / 100.0) + ((17.625 * tempC_dht) / (243.04 + tempC_dht))) / (17.625 - log(humidity / 100.0) - ((17.625 * tempC_dht) / (243.04 + tempC_dht)));
  absoluteHumidity = (actualVaporPressure * 1000.0) / (461.5 * (tempC_dht + 273.15));
  vpd = (saturationVaporPressure - actualVaporPressure) / 1000.0;

  double dryAirPressure = station_pressure_pa - actualVaporPressure;
  airDensity = (dryAirPressure / (287.058 * (tempC_dht + 273.15))) + (actualVaporPressure / (461.495 * (tempC_dht + 273.15)));
  seaLevelPressure = (station_pressure_pa / pow(1.0 - (ALTITUDE_METERS / 44330.0), 5.255)) / 100.0;
}

void printDataToSerial() {
  Serial.println("\n--- Serial Monitor Output ---");
  Serial.println("--- Primary Sensor Readings ---");
  Serial.print("Temperature (DHT11):   "); Serial.print(tempC_dht, 1); Serial.println(" C");
  Serial.print("Temperature (BMP280):  "); Serial.print(tempC_bmp, 1); Serial.println(" C");
  Serial.print("Humidity:              "); Serial.print(humidity, 1); Serial.println(" %");
  Serial.print("Station Pressure:      "); Serial.print(station_pressure_pa / 100.0, 2); Serial.println(" hPa");

  Serial.println("\n--- Calculated Values ---");
  Serial.print("Heat Index:            "); Serial.print(heatIndex, 1); Serial.println(" C");
  Serial.print("Dew Point:             "); Serial.print(dewPoint, 1); Serial.println(" C");
  Serial.print("Absolute Humidity:     "); Serial.print(absoluteHumidity, 2); Serial.println(" g/m^3");
  Serial.print("Vapor Pressure Deficit:"); Serial.print(vpd, 3); Serial.println(" kPa");
  Serial.print("Air Density:           "); Serial.print(airDensity, 4); Serial.println(" kg/m^3");
  Serial.print("Est. Sea Level Pressure:"); Serial.print(seaLevelPressure, 2); Serial.println(" hPa");
  Serial.println("----------------------------------------\n");
}
