#include <ESP8266WiFi.h>

const char* ssid = "RajuPhone";
const char* password = "AkkiAshu3020";

const int RED_PIN = 5;    // D1
const int GREEN_PIN = 4;  // D2
const int BLUE_PIN = 0;   // D3
const int ledPins[] = {RED_PIN, GREEN_PIN, BLUE_PIN};
const char* ledNames[] = {"Red", "Green", "Blue"};
const int numLeds = 3;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println("\nConnecting to WiFi...");

  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH); // All OFF at start (common anode)
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("NodeMCU IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  // --- Read the entire HTTP request ---
  String request = "";
  unsigned long timeout = millis() + 1000;
  while (client.connected() && millis() < timeout) {
    while (client.available()) {
      char c = client.read();
      request += c;
      if (request.endsWith("\r\n\r\n")) break; // End of headers
    }
    if (request.endsWith("\r\n\r\n")) break;
  }
  Serial.print("Full HTTP Request:\n");
  Serial.println(request);

  // --- Parse request line only ---
  int reqEnd = request.indexOf('\r');
  String reqLine = reqEnd != -1 ? request.substring(0, reqEnd) : request;
  Serial.print("Request Line: ");
  Serial.println(reqLine);

  for (int i = 0; i < numLeds; i++) {
    String onCmd = "/led" + String(i+1) + "/on";
    String offCmd = "/led" + String(i+1) + "/off";
    if (reqLine.indexOf(onCmd) != -1) {
      digitalWrite(ledPins[i], LOW);  // ON (common anode)
      sendOk(client);
      return;
    }
    if (reqLine.indexOf(offCmd) != -1) {
      digitalWrite(ledPins[i], HIGH); // OFF (common anode)
      sendOk(client);
      return;
    }
  }

  sendMainPage(client);
}

void sendMainPage(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();

  client.println("<!DOCTYPE html><html><head><title>RGB LED Control</title>");
  client.println("<style>body { font-family: Arial; text-align:center; } .led { margin: 15px; } button { padding: 8px 16px; margin: 5px; font-size: 16px; cursor: pointer; }</style>");
  client.println("<script>");
  client.println("function setLed(num, state) { var xhr = new XMLHttpRequest(); xhr.open('GET', '/led' + num + '/' + state, true); xhr.setRequestHeader('Cache-Control', 'no-cache'); xhr.send(); }");
  client.println("</script>");
  client.println("</head><body>");
  client.println("<h1>RGB LED Control (Common Anode)</h1>");
  for (int i = 0; i < numLeds; i++) {
    client.printf("<div class='led'>%s: ", ledNames[i]);
    client.printf("<button onclick='setLed(%d, \"on\")'>ON</button>", i + 1);
    client.printf("<button onclick='setLed(%d, \"off\")'>OFF</button></div>", i + 1);
  }
  client.println("</body></html>");
  client.stop();
}

void sendOk(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println("Connection: close");
  client.println();
  client.println("OK");
  client.stop();
}
