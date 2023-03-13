// The Proof of Concept: A single cell reed switch + 2 LEDs
#if 0

#define PIN_REED_SWITCH 2
#define PIN_LEDS 13

#include <Arduino.h>
#include "FastLED.h"

CRGB leds[2];
char buff[4];
uint8_t clr[3];
int lastState = HIGH;

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  clr[0] = r;
  clr[1] = g;
  clr[2] = b;
}

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2811, PIN_LEDS, GRB>(leds, 2).setCorrection(UncorrectedColor).setTemperature(HighNoonSun);
  FastLED.setBrightness(210);

  pinMode(PIN_REED_SWITCH, INPUT_PULLUP);
  pinMode(PIN_LEDS, OUTPUT);

  setColor(255, 255, 255);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'r')
      setColor(255, 0, 0);
    else if (c == 'y')
      setColor(255, 255, 0);
    else if (c == 'g')
      setColor(0, 255, 0);
    else if (c == 'b')
      setColor(0, 0, 255);
    else if (c == 'o')
      setColor(255, 150, 0);
    else if (c == 'w')
      setColor(255, 255, 255);
  }

	int reedSwitchState = digitalRead(PIN_REED_SWITCH);
  if (reedSwitchState != lastState) {
    Serial.print("Changed state to ");
    Serial.println(reedSwitchState);
    lastState = reedSwitchState;

    if (reedSwitchState == HIGH) {
      leds[0] = CRGB(0x0);
      leds[1] = CRGB(0x0);
      FastLED.show();
      Serial.println("Off");
    } else {
      leds[0] = CRGB(clr[0], clr[1], clr[2]);
      leds[1] = CRGB(clr[0], clr[1], clr[2]);
      FastLED.show();
      Serial.print("Color ");
      Serial.print(clr[0]);
      Serial.print(" ");
      Serial.print(clr[1]);
      Serial.print(" ");
      Serial.print(clr[2]);
      Serial.println(clr[1]);
    }
  }
}
#endif