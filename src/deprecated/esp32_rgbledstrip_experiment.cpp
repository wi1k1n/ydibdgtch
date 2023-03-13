// The Proof of Concept for ESP32: test FastLED lib with ESP32
#if 0
#define PRFCNPT_PIN_LEDS GPIO_NUM_4
#define PRFCNPT_LEDS_CNT 6

#include <Arduino.h>
#include "FastLED.h"

CRGB leds[PRFCNPT_LEDS_CNT];
char buff[4];
uint8_t clr[3];
int lastState = HIGH;

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  clr[0] = r;
  clr[1] = g;
  clr[2] = b;
}

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, PRFCNPT_PIN_LEDS, GRB>(leds, PRFCNPT_LEDS_CNT).setCorrection(UncorrectedColor).setTemperature(HighNoonSun);
  FastLED.setBrightness(210);

  pinMode(PRFCNPT_PIN_LEDS, OUTPUT);

  setColor(255, 255, 255);
}

uint8_t cursor = 0;
void loop() {
	for (uint8_t i = 0; i < PRFCNPT_LEDS_CNT; ++i)
		leds[i] = i == cursor ? CRGB(clr[0], clr[1], clr[2]) : CRGB(0x0);
	FastLED.show();
	cursor = ++cursor % PRFCNPT_LEDS_CNT;
	delay(1000);
}
#endif