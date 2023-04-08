#include <Arduino.h>
#include "Bounce2.h"

static constexpr int s_debounceIntervalMs = 5;

Bounce button_1;

void setup() {
  button_1.attach(2, INPUT_PULLUP);
  button_1.interval(s_debounceIntervalMs);

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
}