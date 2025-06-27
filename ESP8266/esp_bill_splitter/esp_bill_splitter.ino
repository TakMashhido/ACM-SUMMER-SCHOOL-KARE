#include <ESP8266WiFi.h>

// Replace with your WiFi credentials
const char* ssid = "8501A - 1";
const char* password = "";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi connected!");
  Serial.print("NodeMCU IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  while (!client.available()) delay(1);
  String req = client.readStringUntil('\r');
  client.readStringUntil('\n');

  // Serve Bill Splitter page to any request
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.println("Connection: close");
  client.println();
  client.println(R"rawliteral(
<!DOCTYPE html>
<html lang='en'>
<head>
  <meta charset='UTF-8'>
  <meta name='viewport' content='width=device-width,initial-scale=1.0'>
  <title>Smart Bill Splitter</title>
  <style>
    body {
      background: linear-gradient(135deg, #ffe0e9 0%, #c9fffa 100%);
      font-family: 'Segoe UI', 'Roboto', Arial, sans-serif;
      display: flex; align-items: center; justify-content: center; min-height: 100vh; margin: 0;
    }
    .container {
      background: #fff; border-radius: 18px; box-shadow: 0 4px 32px #0001; padding: 32px 28px; min-width: 320px;
      text-align: center;
    }
    h1 { color: #2e4a65; margin-bottom: 10px;}
    .input-row { margin: 20px 0; display: flex; gap: 10px; justify-content: center; }
    input[type=number] {
      padding: 8px 14px; border: 1.5px solid #e0e0e0; border-radius: 8px; width: 90px; font-size: 1.1em;
      outline: none; transition: border .2s;
    }
    input[type=number]:focus { border-color: #00b2d9; }
    button {
      background: linear-gradient(90deg, #00b2d9 60%, #8f00b3 100%);
      color: #fff; border: none; border-radius: 9px; padding: 10px 28px;
      font-size: 1.05em; letter-spacing: .02em; cursor: pointer; margin-top: 14px;
      box-shadow: 0 2px 8px #0001; transition: transform .1s;
    }
    button:active { transform: scale(.98); }
    .result {
      margin-top: 18px; font-size: 1.25em; color: #008b43; font-weight: 500; letter-spacing: .03em;
      min-height: 2em;
      background: #eafff5; border-radius: 7px; padding: 8px 0;
    }
    .footer {
      margin-top: 25px; font-size: .92em; color: #666;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>🍕 Smart Bill Splitter</h1>
    <form onsubmit="calcSplit(); return false;">
      <div class="input-row">
        <input type="number" min="1" id="total" placeholder="Total Bill ₹">
        <input type="number" min="1" id="people" placeholder="Friends">
      </div>
      <button type="submit">Split It!</button>
    </form>
    <div class="result" id="result">Per person: --</div>
    <div class="footer">by NodeMCU • Enjoy with friends! 🎉</div>
  </div>
  <script>
    function calcSplit() {
      let t = parseFloat(document.getElementById('total').value);
      let p = parseInt(document.getElementById('people').value);
      let res = document.getElementById('result');
      if (isNaN(t) || isNaN(p) || t <= 0 || p <= 0) {
        res.style.color = "#e0002b";
        res.innerHTML = "Enter valid numbers!";
        return;
      }
      let each = t/p;
      res.style.color = "#008b43";
      res.innerHTML = "Per person: ₹ " + each.toFixed(2);
    }
  </script>
</body>
</html>
)rawliteral");
  client.stop();
}
