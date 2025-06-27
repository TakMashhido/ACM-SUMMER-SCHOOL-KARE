#include <ESP8266WiFi.h>

const char* ssid = "RajuPhone";
const char* password = "AkkiAshu3020";

const int RED_PIN = 5;    // D1
const int GREEN_PIN = 4;  // D2
const int BLUE_PIN = 0;   // D3
const int ledPins[] = {RED_PIN, GREEN_PIN, BLUE_PIN};

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("NodeMCU IP address: ");
  Serial.println(WiFi.localIP());

  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH); // OFF (common anode)
  }
  server.begin();
  Serial.println("Server started, waiting for HTTP requests...");
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;
  
  while (!client.available()) delay(1);

  String req = client.readStringUntil('\r');
  client.readStringUntil('\n');

  // Debug: Print every HTTP request to Serial Monitor
  Serial.print("HTTP Request: ");
  Serial.println(req);

  for (int i = 0; i < 3; i++) {
    String onCmd = "/led" + String(i+1) + "/on";
    String offCmd = "/led" + String(i+1) + "/off";
    if (req.indexOf(onCmd) != -1) {
      digitalWrite(ledPins[i], LOW);  // ON (common anode)
      client.println("HTTP/1.1 200 OK\r\nConnection: close\r\n\r\nOK");
      Serial.print("LED "); Serial.print(i+1); Serial.println(" ON");
      client.stop();
      return;
    }
    if (req.indexOf(offCmd) != -1) {
      digitalWrite(ledPins[i], HIGH); // OFF (common anode)
      client.println("HTTP/1.1 200 OK\r\nConnection: close\r\n\r\nOK");
      Serial.print("LED "); Serial.print(i+1); Serial.println(" OFF");
      client.stop();
      return;
    }
  }
  client.println("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<h1>RGB Controller</h1>");
  client.stop();
}
