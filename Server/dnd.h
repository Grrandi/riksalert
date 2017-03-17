
const int statusLed = D3;

// for led fade in and fade out
int statusBrightness = 0;
int fadeAmount = 5;

// status led fade in fade out 
unsigned long previousMillis = 0;
long statusBlinkDelay = 40;

// do not disturb timer in milliseconds
const int dndTime = 10000;
int dndPreviousMillis = 0;

unsigned long currentMillis = 0;
unsigned long dndUntil = 0;

const int dndButton = D4;

void setupDnd() {
  Serial.println("setup dnd");
  pinMode(statusLed, OUTPUT);
  pinMode(dndButton, INPUT);
}

void blinkStatusLed() {
  analogWrite(statusLed, statusBrightness);
  statusBrightness = statusBrightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (statusBrightness <= 0 || statusBrightness >= 255) {
    fadeAmount = -fadeAmount;
  }
}

int isDnDActive() {
  return dndUntil > currentMillis;
}

void handleDoNotDisturb() {
  currentMillis = millis();
  if (!digitalRead(dndButton)) {
    dndUntil = currentMillis + dndTime;
  }
  if (isDnDActive()) {
    if(currentMillis - previousMillis > statusBlinkDelay) {
      previousMillis = currentMillis;
      blinkStatusLed();
    }
  }
  else {
    analogWrite(statusLed, LOW);
  }
}
