const int SOUND_SENSOR_PIN = 2;  // Sound sensor OUT pin
const int LED_PIN = 8;          // LED pin

unsigned long lastClapTime = 0;
const unsigned long doubleClapGap = 700; // Max ms between claps for double clap

bool ledState = false; // LED OFF at start
bool waitingForSecondClap = false;

void ledOn()  { digitalWrite(LED_PIN, LOW); }  // For common anode
void ledOff() { digitalWrite(LED_PIN, HIGH); }

void setup() {
  pinMode(SOUND_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  ledOff();
}

void loop() {
  static bool prevSound = false;
  bool soundNow = digitalRead(SOUND_SENSOR_PIN);

  // Detect rising edge (from LOW to HIGH, i.e., new clap)
  if (soundNow && !prevSound) {
    unsigned long now = millis();
    if (!ledState) {
      // LED is OFF, first clap turns it ON and starts waiting for double clap
      ledOn();
      ledState = true;
      waitingForSecondClap = true;
      lastClapTime = now;
    } else if (ledState && waitingForSecondClap && (now - lastClapTime < doubleClapGap)) {
      // LED is ON, and this is the second clap in time window → turn OFF
      ledOff();
      ledState = false;
      waitingForSecondClap = false;
    } else if (ledState && !waitingForSecondClap) {
      // If LED is ON and we're not waiting, start window for double clap
      waitingForSecondClap = true;
      lastClapTime = now;
    }
  }

  // If waiting for double clap, timeout after the gap
  if (waitingForSecondClap && millis() - lastClapTime > doubleClapGap) {
    waitingForSecondClap = false;
  }

  prevSound = soundNow;
}
