#include <Arduino.h>
#include "Bounce2.h"

static constexpr int s_debounceIntervalMs = 5;

static uint32_t lastJoystickX = 0;
static uint32_t lastJoystickY = 0;

Bounce button_1;

static constexpr int s_joystickXPin = 14;
static constexpr int s_joystickYPin = 15;

void setup() {
  button_1.attach(2, INPUT_PULLUP);
  button_1.interval(s_debounceIntervalMs);

  pinMode(s_joystickXPin, INPUT);
  pinMode(s_joystickYPin, INPUT);

  Serial.begin(115200);
  while (!Serial && millis() < 5000)
    ;

  Serial.println("Hello world!");
}

void loop() {
  button_1.update();

  if (button_1.fell()) {
    Serial.println("Button is pressed");
  }

  if (button_1.rose()) {
    Serial.println("Button is released");
  }

  const auto joystickX = analogRead(s_joystickXPin) / 8;
  const auto joystickY = analogRead(s_joystickYPin) / 8;

  if (joystickX != lastJoystickX || joystickY != lastJoystickY) {
    Serial.printf("X: %d, Y: %d\n", joystickX, joystickY);
    lastJoystickX = joystickX;
    lastJoystickY = joystickY;
  }
}