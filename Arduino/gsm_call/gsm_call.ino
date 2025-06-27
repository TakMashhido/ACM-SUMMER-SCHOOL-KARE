#include <SoftwareSerial.h>

SoftwareSerial SIM900A(10, 11); // RX, TX

void setup() {
  SIM900A.begin(9600);   // GSM baud rate
  Serial.begin(9600);    // Serial Monitor baud rate
  Serial.println("SIM900A Ready");

  SIM900A.println("AT");
  delay(500);
  Serial.print("AT Response: "); printGsmResponse();

  SIM900A.println("AT+CPIN?");
  delay(500);
  Serial.print("SIM Status: "); printGsmResponse();

  SIM900A.println("AT+CSQ");
  delay(500);
  Serial.print("Signal Quality: "); printGsmResponse();

  Serial.println("Type 's' to send message, 'r' to receive message, 'c' to call, 'h' to hang up");
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
      case 'c':
        MakeCall();
        break;
      case 'h':
        HangUp();
        break;
    }
  }

  if (SIM900A.available() > 0)
    Serial.write(SIM900A.read());
}

void SendMessage() {
  Serial.println("Sending Message...");
  SIM900A.println("AT+CMGF=1");
  delay(1000);
  SIM900A.println("AT+CMGS=\"+919247193234\""); // Change number as needed
  delay(1000);
  SIM900A.println("Good morning, how are you doing?");
  delay(500);
  SIM900A.write(26); // CTRL+Z
  delay(2000);
  Serial.println("Message has been sent.");
}

void ReceiveMessage() {
  Serial.println("Setting up to receive SMS...");
  SIM900A.println("AT+CNMI=2,2,0,0,0");
  delay(1000);
  Serial.println("Ready to receive messages.");
}

void MakeCall() {
  Serial.println("Dialing number...");
  SIM900A.println("ATD+919247193234;"); // Don't forget the semicolon
  delay(1000);
  Serial.println("Call command sent. Type 'h' to hang up.");
}

void HangUp() {
  Serial.println("Hanging up...");
  SIM900A.println("ATH");
  delay(500);
  Serial.println("Hangup command sent.");
}

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
