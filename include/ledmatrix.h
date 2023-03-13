#ifndef LEDMATRIX_H__
#define LEDMATRIX_H__

#include "constants.h"

#include <Arduino.h>
#include "FastLED.h"

const uint8_t _LEDMATRIX_LEDS_COUNT = 8 * 8 * LEDMATRIX_CONSEQUENT_LEDS_COUNT;

struct CellCRGB {
	static uint8_t ledCount;
	CRGB colors[LEDMATRIX_CONSEQUENT_LEDS_COUNT];

	CellCRGB() = default;
	CellCRGB(const CRGB& clr);
};

class LEDMatrix {
	CRGB _leds[_LEDMATRIX_LEDS_COUNT];
public:
	LEDMatrix() = default;
	bool init();

	void showLEDs(CellCRGB (*check)(uint8_t idx));
};

#endif // LEDMATRIX_H__