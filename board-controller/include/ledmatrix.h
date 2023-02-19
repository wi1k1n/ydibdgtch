#ifndef LEDMATRIX_H__
#define LEDMATRIX_H__

#include "constants.h"

#include <Arduino.h>
#include "FastLED.h"

const uint8_t _LEDMATRIX_LEDS_COUNT = 8 * 8 * LEDMATRIX_CONSEQUENT_LEDS_COUNT;

class LEDMatrix {
	CRGB _leds[_LEDMATRIX_LEDS_COUNT];
public:
	LEDMatrix() = default;
	bool begin();

	void showLEDs(CRGB (*check)(uint8_t idx));
};

#endif // LEDMATRIX_H__