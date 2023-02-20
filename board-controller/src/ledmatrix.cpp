#include "ledmatrix.h"

uint8_t CellCRGB::ledCount = LEDMATRIX_CONSEQUENT_LEDS_COUNT;
CellCRGB::CellCRGB(const CRGB& clr) {
	for (uint8_t j = 0; j < CellCRGB::ledCount; ++j)
		colors[j] = clr;
}

bool LEDMatrix::begin() {
	FastLED.addLeds<WS2811, PIN_LEDS, GRB>(_leds, _LEDMATRIX_LEDS_COUNT).setCorrection(TypicalSMD5050).setTemperature(HighNoonSun); // UncorrectedColor
	FastLED.setBrightness(10);
	pinMode(PIN_LEDS, OUTPUT);

	return true;
}

void LEDMatrix::showLEDs(CellCRGB (*check)(uint8_t idx)) {
#ifdef _DEBUG_
	const uint8_t dbgLedsCols = 3;
	const uint8_t dbgLedsRows = 2;
#endif
	for (uint8_t i = 0; i < 64; ++i) {
		const uint8_t row = i / 8;
		uint8_t col = i % 8;
		bool isRowOdd = row % 2;
		if (isRowOdd) {
			col = 7 - col;
#ifdef _DEBUG_
			col = dbgLedsCols - 1 - i % 8;
#endif
		}
#ifdef _DEBUG_
		if (col >= dbgLedsCols || row >= dbgLedsRows)
			continue;
#endif
		CellCRGB cell = check(i);
        uint8_t cellLedIdx = row * 8 + col; // index of current cell in led coordinate system
		for (uint8_t j = 0; j < CellCRGB::ledCount; ++j) {
			// uint8_t ledLocalIdx = isRowOdd ? (CellCRGB::ledCount - 1 - j) : j; // if leds order in each cell was reverted
			uint8_t ledLocalIdx = j;
			_leds[cellLedIdx * CellCRGB::ledCount + ledLocalIdx] = cell.colors[j];
		}
	}
	FastLED.show();
}
void LEDMatrix::showLEDs(CRGB (*check)(uint8_t idx)) {
	showLEDs(check);
}