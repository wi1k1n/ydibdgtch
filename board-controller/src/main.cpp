// YDIBDGTCHB main code file

#include "constants.h"

#include <Arduino.h>

#include "board.h"
#include "ledmatrix.h"
#include "communication.h"

Board board;
LEDMatrix leds;
Communication communication;

void setup() {
	communication.begin(0x8);
	board.begin();
	leds.begin();
}

CellCRGB setLEDColor(uint8_t idx) {
	bool val = board.getState(idx);
	return val ? CRGB(0xFFFFFF) : CRGB(0x0);
}

void loop() {
	board.scan();
	communication.tick();
	leds.showLEDs(&setLEDColor);
}