#include <Arduino.h>
#include <array>
#include "Bounce2.h"

static constexpr int s_debounceIntervalMs = 5;

static uint32_t lastJoystickX = 0;
static uint32_t lastJoystickY = 0;

static std::array<Bounce, 10> buttons;

static constexpr int s_joystickXPin = 15;
static constexpr int s_joystickYPin = 14;

void setup() {
  for (auto i = 0; i < buttons.size(); ++i) {
    buttons[i].attach(2 + i, INPUT_PULLUP);
    buttons[i].interval(s_debounceIntervalMs);
  }

  pinMode(s_joystickXPin, INPUT);
  pinMode(s_joystickYPin, INPUT);

  Serial.begin(115200);
  while (!Serial && millis() < 5000)
    ;

  Serial.println("Hello world!");
}

void loop() {
  for (auto i = 0; i < buttons.size(); ++i) {
    buttons[i].update();

    if (buttons[i].fell()) {
      Serial.printf("Button %d is pressed\n", i);
    }

    if (buttons[i].rose()) {
      Serial.printf("Button %d is released\n", i);
    }
  }

  const auto joystickX = analogRead(s_joystickXPin) / 8;
  const auto joystickY = analogRead(s_joystickYPin) / 8;

  if (joystickX != lastJoystickX || joystickY != lastJoystickY) {
    Serial.printf("X: %d, Y: %d\n", joystickX, joystickY);
    lastJoystickX = joystickX;
    lastJoystickY = joystickY;
  }
}