#include "ledmatrix.h"

bool LEDMatrix::begin() {
	FastLED.addLeds<WS2811, PIN_LEDS, GRB>(_leds, _LEDMATRIX_LEDS_COUNT).setCorrection(UncorrectedColor).setTemperature(HighNoonSun);
	FastLED.setBrightness(5);
	pinMode(PIN_LEDS, OUTPUT);

	return true;
}

void LEDMatrix::showLEDs(CRGB (*check)(uint8_t idx)) {
	// temp fix while board is only partially finished
	uint8_t tIdx = 0;
	for (uint8_t i = 0; i < 64; ++i) {
		if (i % 8 >= 3 || i > 10)
			continue;
		CRGB clr = check(i);
		_leds[tIdx * LEDMATRIX_CONSEQUENT_LEDS_COUNT] = clr;
		_leds[tIdx * LEDMATRIX_CONSEQUENT_LEDS_COUNT + 1] = clr;
		++tIdx;
	}
	FastLED.show();
}