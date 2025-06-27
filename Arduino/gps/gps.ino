#include <SoftwareSerial.h>
SoftwareSerial gpsSerial(8, 9); // RX, TX (use only TX from GPS)

void setup() {
  Serial.begin(9600);      // For Serial Monitor
  gpsSerial.begin(9600);   // For GPS module
  Serial.println("GPS Test: Waiting for data...");
}

void loop() {
  while (gpsSerial.available()) {
    char c = gpsSerial.read();
    Serial.write(c); // Forward GPS data to Serial Monitor
  }
}
