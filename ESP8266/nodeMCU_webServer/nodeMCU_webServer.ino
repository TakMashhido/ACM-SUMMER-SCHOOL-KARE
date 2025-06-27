#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "RajuPhone";
const char* password = "AkkiAshu3020  ";

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected!");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    server.send(200, "text/plain", "Hello from NodeMCU!");
  });

  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
}
