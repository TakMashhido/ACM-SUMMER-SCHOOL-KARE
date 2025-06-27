#include <SoftwareSerial.h>

SoftwareSerial SIM900A(10, 11); // RX, TX

void setup() {
  SIM900A.begin(9600);   // GSM baud rate
  Serial.begin(9600);    // Serial Monitor baud rate
  Serial.println("SIM900A Ready");

  // ----- SIM Status -----
  SIM900A.println("AT"); // Basic check
  delay(500);
  Serial.print("AT Response: ");
  printGsmResponse();

  SIM900A.println("AT+CPIN?"); // SIM status
  delay(500);
  Serial.print("SIM Status: ");
  printGsmResponse();

  SIM900A.println("AT+CSQ"); // Signal quality
  delay(500);
  Serial.print("Signal Quality: ");
  printGsmResponse();

  Serial.println("Type 's' to send message or 'r' to receive message");
}

void loop() {
  if (Serial.available() > 0) {
    switch (Serial.read()) {
      case 's':
        SendMessage();
        break;
      case 'r':
        ReceiveMessage();
        break;
    }
  }

  if (SIM900A.available() > 0)
    Serial.write(SIM900A.read());
}

void SendMessage() {
  Serial.println("Sending Message...");
  SIM900A.println("AT+CMGF=1");    // Set SMS to text mode
  delay(1000);
  SIM900A.println("AT+CMGS=\"+919291548210\""); // Recipient number
  delay(1000);
  SIM900A.println("Good morning.!"); // SMS text
  delay(500);
  SIM900A.write(26); // CTRL+Z
  delay(2000);
  Serial.println("Message has been sent.");
}

void ReceiveMessage() {
  Serial.println("Setting up to receive SMS...");
  SIM900A.println("AT+CNMI=2,2,0,0,0"); // Set to display incoming SMS immediately
  delay(1000);
  Serial.println("Ready to receive messages.");
}

// Utility: Print SIM900A response to Serial Monitor
void printGsmResponse() {
  unsigned long timeout = millis() + 2000;
  while (millis() < timeout) {
    if (SIM900A.available()) {
      char c = SIM900A.read();
      Serial.write(c);
    }
  }
  Serial.println();
}
