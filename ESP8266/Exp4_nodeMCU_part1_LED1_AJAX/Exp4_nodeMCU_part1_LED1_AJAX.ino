#include <ESP8266WiFi.h>

// WiFi credentials
const char* ssid = "RajuPhone";
const char* password = "AkkiAshu3020"; 

// RGB LED pins (D1=GPIO5=Red, D2=GPIO4=Green, D3=GPIO0=Blue)
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

  while (!client.available()) delay(1);
  String req = client.readStringUntil('\r');
  client.readStringUntil('\n'); // consume \n

  Serial.print("Received request: ");
  Serial.println(req);

  for (int i = 0; i < numLeds; i++) {
    String onCmd = "/led" + String(i+1) + "/on";
    String offCmd = "/led" + String(i+1) + "/off";
    if (req.indexOf(onCmd) != -1) {
      digitalWrite(ledPins[i], LOW);  // ON (common anode)
      sendOk(client);
      return;
    }
    if (req.indexOf(offCmd) != -1) {
      digitalWrite(ledPins[i], HIGH); // OFF (common anode)
      sendOk(client);
      return;
    }
  }

  sendMainPage(client); // Serve main page for any other request
}

void sendMainPage(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();

  client.println("<!DOCTYPE html><html><head><title>RGB LED Control</title>");
  client.println("<style>");
  client.println("body { font-family: Arial; text-align:center; }");
  client.println(".led { margin: 15px; }");
  client.println("button { padding: 8px 16px; margin: 5px; font-size: 16px; cursor: pointer; }");
  client.println("</style>");
  client.println("<script>");
  client.println("function setLed(num, state) {");
  client.println("  var xhr = new XMLHttpRequest();");
  client.println("  xhr.open('GET', '/led' + num + '/' + state, true);");
  client.println("  xhr.setRequestHeader('Cache-Control', 'no-cache');");
  client.println("  xhr.send();");
  client.println("}");
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
