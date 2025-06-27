#include <ESP8266WiFi.h>

const char* ssid = "RajuPhone";
const char* password = "AkkiAshu3020";

WiFiServer server(80);

const int ledPins[] = {5, 4, 0};  // GPIO5=D1, GPIO4=D2, GPIO0=D3
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  while(!client.available()) delay(1);
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Control LEDs based on URL request
  for (int i = 0; i < numLeds; i++) {
    String ledOn = "/LED" + String(i+1) + "=ON";
    String ledOff = "/LED" + String(i+1) + "=OFF";

    if (request.indexOf(ledOn) != -1) {
      digitalWrite(ledPins[i], HIGH);
    } else if (request.indexOf(ledOff) != -1) {
      digitalWrite(ledPins[i], LOW);
    }
  }

  // Send HTTP response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<h1>NodeMCU LED Control with Status</h1>");

  for (int i = 0; i < numLeds; i++) {
    client.print("<p>LED ");
    client.print(i+1);
    client.print(": <a href=\"/LED");
    client.print(i+1);
    client.print("=ON\">ON</a> | <a href=\"/LED");
    client.print(i+1);
    client.print("=OFF\">OFF</a> | Status: ");

    // Show LED status based on pin state
    if (digitalRead(ledPins[i]) == HIGH) {
      client.print("<b>ON</b>");
    } else {
      client.print("OFF");
    }
    client.println("</p>");
  }

  client.println("</html>");
  delay(1);
  client.stop();
}
