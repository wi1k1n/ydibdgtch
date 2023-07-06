#ifndef LEDMATRIX_H__
#define LEDMATRIX_H__

#include "constants.h"

#include <Arduino.h>
#include "FastLED.h"

#include <functional>

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

	// void clear();
	// void setAt(const CellCRGB& clr);

	// memory efficient way to set leds, but sometimes inconvenient
	void showLEDs(std::function<CellCRGB(uint8_t)> check);
};

class LEDEffect {

};

#endif // LEDMATRIX_H__