#include <Servo.h>

const int pinLatch = D0; // latch
const int pinClock = D1; // clock
const int pinData  = D2; // data

const int pinServo = D8;

const int disturbDuration = 15000;

unsigned long tickMs;
unsigned long disturbUntil = 0;
unsigned long nextBlink = 0;
unsigned long nextServoStep = 0;

const int blinkDelay = 50;
byte ledPattern = 0;

const int servoDelay = 650;
int lastServoState = 0;

unsigned int fullPattern = 0xFF00;
unsigned int shiftNum = 0;
int shiftShift = 1;

Servo martti;

void setupDisturb() {
  //set pins to output because they are addressed in the main loop
  Serial.println("setup halarm");
  pinMode(pinLatch, OUTPUT);
  pinMode(pinData, OUTPUT);  
  pinMode(pinClock, OUTPUT);
}

int shouldDisturb() {
  return tickMs < disturbUntil;
}

void resetData() {
    shiftNum = 0;
    shiftShift = 1;
}

void setLeds(const byte leds) {
  digitalWrite(pinLatch, LOW);
  shiftOut(pinData, pinClock, MSBFIRST, leds);
  digitalWrite(pinLatch, HIGH);
}

void startDisturb() {
  if (disturbUntil == 0) {
    resetData();
    setLeds(0);
    // servo
    martti.attach(pinServo);
    martti.write(0);
  }
  disturbUntil = millis() + disturbDuration;
}

void stopDisturb() {
  if (disturbUntil != 0) {
    resetData();
    setLeds(0);
    // servo
    martti.write(0);
    martti.detach();
  }
  disturbUntil = 0;
}

void advanceServo() {
  nextServoStep = tickMs + servoDelay;
  lastServoState = ~lastServoState & 1;
  martti.write(lastServoState * 180);
}

void advanceBlink() {
  nextBlink = tickMs + 50;
  if (shiftNum > 16 || shiftNum < 0) {
    shiftShift = 0 - shiftShift;
  }
  ledPattern = fullPattern >> shiftNum;
  shiftNum += shiftShift;
  setLeds(ledPattern);
}

void tickDisturb() {
  tickMs = millis();
  if (shouldDisturb()) {
    if (tickMs >= nextBlink) {
      advanceBlink();
    }
    if (tickMs >= nextServoStep) {
      advanceServo();
    }
  } else {
    stopDisturb();
  }
}

