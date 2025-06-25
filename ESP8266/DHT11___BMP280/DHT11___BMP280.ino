#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "DHT.h"
#include <math.h>

// =================================================================
// ACTION: Set your location's altitude in meters for accurate Sea Level Pressure
// An estimate for the Krishnan Kovil area is used here.
// =================================================================
#define ALTITUDE_METERS 140.0

// --- Sensor Objects ---
Adafruit_BMP280 bmp;
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- Global variables to hold all sensor & calculated data ---
float humidity = 0.0, tempC_dht = 0.0, tempC_bmp = 0.0;
float station_pressure_pa = 0.0, heatIndex = 0.0, dewPoint = 0.0;
float absoluteHumidity = 0.0, vpd = 0.0, airDensity = 0.0, seaLevelPressure = 0.0;

// =================================================================
// SETUP: Runs once on boot
// =================================================================
void setup() {
  Serial.begin(9600);
  // Wait for serial to connect
  while (!Serial); 
  
  Serial.println("Advanced Weather Station - Serial Monitor Output");
  initSensors();
}

// =================================================================
// LOOP: Runs continuously
// =================================================================
void loop() {
  // Step 1: Read sensors and perform all calculations
  updateSensorData();

  // Step 2: Print all the data to the Serial Monitor
  printDataToSerial();

  // Step 3: Wait for 5 seconds before the next reading
  delay(1000);
}

// =================================================================
// SENSOR & DATA FUNCTIONS
// =================================================================

void initSensors() {
  // Initialize BMP280 Sensor
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BMP280 sensor! Check wiring or I2C address.");
    while (1);
  }
  
  // Initialize DHT11 Sensor
  dht.begin();
  Serial.println("Sensors Initialized. Starting readings...");
}

void updateSensorData() {
  // --- Read Primary Sensors ---
  float newHum = dht.readHumidity();
  float newTemp = dht.readTemperature();

  // Check if reads failed. If so, keep old data and return.
  if (isnan(newHum) || isnan(newTemp)) {
    Serial.println("Failed to read from DHT sensor! Retrying...");
    return;
  }
  
  humidity = newHum;
  tempC_dht = newTemp;
  tempC_bmp = bmp.readTemperature();
  station_pressure_pa = bmp.readPressure(); // Get pressure in Pascals (Pa)

  // --- Perform All Calculations ---
  // Intermediate values needed for multiple calculations
  double saturationVaporPressure = 611.2 * exp((17.67 * tempC_dht) / (tempC_dht + 243.5)); // in Pa
  double actualVaporPressure = saturationVaporPressure * (humidity / 100.0); // in Pa
  
  // Basic calculations
  heatIndex = dht.computeHeatIndex(tempC_dht, humidity, false);
  dewPoint = calculateDewPoint(tempC_dht, humidity);

  // --- Advanced Calculations ---
  absoluteHumidity = (actualVaporPressure * 1000.0) / (461.5 * (tempC_dht + 273.15));
  vpd = (saturationVaporPressure - actualVaporPressure) / 1000.0; // in kPa
  
  double dryAirPressure = station_pressure_pa - actualVaporPressure;
  airDensity = (dryAirPressure / (287.058 * (tempC_dht + 273.15))) + (actualVaporPressure / (461.495 * (tempC_dht + 273.15)));
  
  seaLevelPressure = (station_pressure_pa / pow(1.0 - (ALTITUDE_METERS / 44330.0), 5.255)) / 100.0; // convert to hPa
}

float calculateDewPoint(float temp, float hum) {
  // Magnus-Tetens formula for calculating dew point
  return 243.04 * (log(hum / 100.0) + ((17.625 * temp) / (243.04 + temp))) / (17.625 - log(hum / 100.0) - ((17.625 * temp) / (243.04 + temp)));
}

void printDataToSerial() {
  Serial.println("----------------------------------------");
  Serial.println("         Primary Sensor Readings");
  Serial.println("----------------------------------------");
  Serial.print("Temperature (DHT11):    "); Serial.print(tempC_dht, 1); Serial.println(" C");
  Serial.print("Temperature (BMP280):   "); Serial.print(tempC_bmp, 1); Serial.println(" C");
  Serial.print("Humidity:               "); Serial.print(humidity, 1); Serial.println(" %");
  Serial.print("Station Pressure:       "); Serial.print(station_pressure_pa / 100.0, 2); Serial.println(" hPa");

  Serial.println("\n----------------------------------------");
  Serial.println("            Calculated Values");
  Serial.println("----------------------------------------");
  Serial.print("Heat Index:             "); Serial.print(heatIndex, 1); Serial.println(" C");
  Serial.print("Dew Point:              "); Serial.print(dewPoint, 1); Serial.println(" C");
  Serial.print("Absolute Humidity:      "); Serial.print(absoluteHumidity, 2); Serial.println(" g/m^3");
  Serial.print("Vapor Pressure Deficit: "); Serial.print(vpd, 3); Serial.println(" kPa");
  Serial.print("Air Density:            "); Serial.print(airDensity, 4); Serial.println(" kg/m^3");
  Serial.print("Est. Sea Level Pressure:"); Serial.print(seaLevelPressure, 2); Serial.println(" hPa");
  Serial.println("----------------------------------------\n");
}
