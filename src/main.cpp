#include <Arduino.h>
#include <MIDI.h>
#include <array>
#include "Bounce2.h"

#define PIN_BUTTON_1 2
#define PIN_BUTTON_JOYSTICK 12
#define PIN_JOYSTICK_X 15
#define PIN_JOYSTICK_Y 14

static constexpr int s_debounceIntervalMs = 5;

static int joystickCenterX = 0;
static int joystickCenterY = 0;
static Bounce joystickButton;

static std::array<Bounce, 10> buttons;
constexpr std::array<uint8_t, 10> s_buttonsToMidiNotes {40, 41, 43, 45, 46, 44, 42, 37, 39, 38};

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
constexpr int s_midiChannel = 6;

void playStartupSequence()
{
  for (size_t i = 0; i < buttons.size(); ++i) {
    pinMode(PIN_BUTTON_1 + i, OUTPUT_OPENDRAIN);
    delayMicroseconds(1000*100);
  }

  for (size_t i = 0; i < buttons.size(); ++i) {
    pinMode(PIN_BUTTON_1 + i, INPUT_PULLUP);
    delayMicroseconds(1000*100);
  }
}

uint8_t getMappedJoystickValue(int rawValue, int centerValue)
{
  constexpr uint8_t s_targetCenterValue = 64;
  constexpr int s_joystickCenterZone = 1;

    if (abs(rawValue - centerValue) <= s_joystickCenterZone) {
      return s_targetCenterValue;
    } else if (rawValue < centerValue) {
      const auto mappedValue = map(rawValue, 0, centerValue - s_joystickCenterZone, 0, s_targetCenterValue);
      return uint8_t(max(mappedValue, 0));
    } else {
      const auto mappedValue = map(rawValue, centerValue + s_joystickCenterZone, 1023, s_targetCenterValue, 127);
      return uint8_t(min(mappedValue, 127));
    }
}

void setup()
{
  playStartupSequence();

  for (size_t i = 0; i < buttons.size(); ++i) {
    buttons[i].attach(2 + i, INPUT_PULLUP);
    buttons[i].interval(s_debounceIntervalMs);
  }
  joystickButton.attach(PIN_BUTTON_JOYSTICK, INPUT_PULLUP);
  joystickButton.interval(s_debounceIntervalMs);

  pinMode(PIN_JOYSTICK_X, INPUT);
  pinMode(PIN_JOYSTICK_Y, INPUT);

  delayMicroseconds(1000);

  joystickCenterX = analogRead(PIN_JOYSTICK_X);
  joystickCenterY = analogRead(PIN_JOYSTICK_Y);

  Serial.begin(115200);
  while (!Serial && millis() < 5000)
    ;

  Serial.printf("Joystick center values\nX: %d, Y: %d\n", joystickCenterX, joystickCenterY);

  MIDI.begin();
}

void loop()
{
  for (size_t i = 0; i < buttons.size(); ++i) {
    buttons[i].update();

    if (buttons[i].fell()) {
      Serial.printf("Button %d is pressed\n", i);

      MIDI.sendNoteOn(s_buttonsToMidiNotes[i], 127, s_midiChannel);
    }

    if (buttons[i].rose()) {
      Serial.printf("Button %d is released\n", i);

      MIDI.sendNoteOff(s_buttonsToMidiNotes[i], 127, s_midiChannel);
    }
  }

  joystickButton.update();
  if (joystickButton.fell()) {
      Serial.println("Joystick button is pressed");
  }

  if (joystickButton.rose()) {
      Serial.println("Joystick button is released");
  }

  const auto joystickX = analogRead(PIN_JOYSTICK_X);
  const auto joystickY = analogRead(PIN_JOYSTICK_Y);

  const auto mappedX = getMappedJoystickValue(joystickX, joystickCenterX);
  const auto mappedY = getMappedJoystickValue(joystickY, joystickCenterY);

  static uint8_t lastJoystickX = 0;
  static uint8_t lastJoystickY = 0;

  if (mappedX != lastJoystickX) {
    lastJoystickX = mappedX;

    Serial.printf("X CC: %d (raw: %d)\n", mappedX, joystickX);
    MIDI.sendControlChange(77, mappedX, s_midiChannel);
  }

  if (mappedY != lastJoystickY) {
    lastJoystickY = mappedY;

    Serial.printf("Y CC: %d (raw: %d)\n", mappedY, joystickY);
    MIDI.sendAfterTouch(mappedY, s_midiChannel);
  }
}