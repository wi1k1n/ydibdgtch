// YDIBDGTCHB main code file

#include "constants.h"

#include <Arduino.h>

#include "board.h"
#include "ledmatrix.h"

Board board;
LEDMatrix leds;

void setup() {
	Serial.begin(9600);

	board.begin();
	leds.begin();
}

CRGB setLEDColor(uint8_t idx) {
	bool val = board.getState(idx);
	return val ? CRGB(0xFFFFFF) : CRGB(0x0);
}

void loop() {
	board.scan();
	// board.print();
	leds.showLEDs(&setLEDColor);
	delay(10);
}