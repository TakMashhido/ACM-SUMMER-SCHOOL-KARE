// Include the ESP8266 WiFi library
#include <ESP8266WiFi.h>

// --- WiFi Network Credentials ---
// Replace the following with your WiFi network's name (SSID) and password.
const char* ssid = "KLU_WIFI";
const char* password = "";
// --------------------------------

// --- (Optional) Static IP Configuration ---
// Uncomment the following lines to use a static IP address.
// IPAddress staticIP(192, 168, 1, 184); // Set your desired static IP
// IPAddress gateway(192, 168, 1, 1);    // Set your network gateway
// IPAddress subnet(255, 255, 255, 0);   // Set your network subnet mask
// IPAddress primaryDNS(8, 8, 8, 8);     // Optional: Google's public DNS
// IPAddress secondaryDNS(8, 8, 4, 4);   // Optional: Google's public DNS
// ------------------------------------

void connectToWiFi() {
  Serial.println("");
  Serial.print("Connecting to WiFi network: ");
  Serial.println(ssid);

  // Set the ESP8266 to be a WiFi station (client)
  WiFi.mode(WIFI_STA);
  
  // --- Check if Static IP is defined ---
  // The 'staticIP' variable is only defined if you uncomment the section above.
  #ifdef staticIP
    // Configure the static IP address
    if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) {
      Serial.println("STA Failed to configure");
    }
  #endif
  
  // Initiate the connection
  WiFi.begin(ssid, password);

  // Wait for the connection to complete, with a timeout
  int connection_timeout = 20; // Timeout in seconds (20 * 500ms = 10 seconds)
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Wait 500 milliseconds
    Serial.print(".");
    connection_timeout--;
    if (connection_timeout <= 0) {
      Serial.println("\n\nFailed to connect to WiFi. Please check credentials or network.");
      // You could have the device sleep or restart here
      return; // Exit the function if connection fails
    }
  }

  // If the loop is exited, it means the connection was successful
  Serial.println("");
  Serial.println("WiFi connected successfully!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // Initialize Serial communication at 115200 baud rate for status messages
  Serial.begin(115200);
  delay(10); // A small delay to allow the serial port to initialize

  connectToWiFi(); // Initial attempt to connect
}

void loop() {
  // Check the WiFi connection status in the main loop.
  // If the connection is lost, it will attempt to reconnect.
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi connection lost. Reconnecting...");
    connectToWiFi();
  }
  
  // You can add your main application code here.
  // This section will only run if the WiFi is connected.
  
  // Example: Print a message every 5 seconds
  // Serial.println("Main loop is running...");
  // delay(5000);
}
