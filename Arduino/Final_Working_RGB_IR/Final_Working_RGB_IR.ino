#include <IRremote.h>

// Pin definitions for RGB LED
#define RED_PIN    9
#define GREEN_PIN  10
#define BLUE_PIN   11
#define IR_RECEIVE_PIN 7

// Replace with your actual button codes!
#define RED_BUTTON_CODE    0xA956FE01
#define GREEN_BUTTON_CODE  0xA55AFE01
#define BLUE_BUTTON_CODE   0xB946FE01
#define OFF_BUTTON_CODE    0xBF40FE01

IRrecv irrecv(IR_RECEIVE_PIN);

// State variables
bool redState = false;
bool greenState = false;
bool blueState = false;

void setup() {
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // All LEDs OFF at start
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);

  irrecv.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

void loop() {
  if (irrecv.decode()) {
    unsigned long codeValue = irrecv.decodedIRData.decodedRawData;

    // Ignore repeat and invalid codes
    if (codeValue == 0xFFFFFFFF || codeValue == 0x0) {
      irrecv.resume();
      return;
    }

    Serial.print("IR Code: 0x");
    Serial.println(codeValue, HEX);

    if (codeValue == RED_BUTTON_CODE) {
      redState = !redState; // Toggle red state
      digitalWrite(RED_PIN, redState ? LOW : HIGH);  // LOW = ON for common anode
      Serial.println(redState ? "RED ON" : "RED OFF");
    }
    else if (codeValue == GREEN_BUTTON_CODE) {
      greenState = !greenState; // Toggle green state
      digitalWrite(GREEN_PIN, greenState ? LOW : HIGH);  // LOW = ON
      Serial.println(greenState ? "GREEN ON" : "GREEN OFF");
    }
    else if (codeValue == BLUE_BUTTON_CODE) {
      blueState = !blueState; // Toggle blue state
      digitalWrite(BLUE_PIN, blueState ? LOW : HIGH);  // LOW = ON
      Serial.println(blueState ? "BLUE ON" : "BLUE OFF");
    }
    else if (codeValue == OFF_BUTTON_CODE) {
      redState = false;
      greenState = false;
      blueState = false;
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(BLUE_PIN, HIGH);
      Serial.println("ALL OFF");
    }

    irrecv.resume();
  }
}
