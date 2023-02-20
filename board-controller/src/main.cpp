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
#ifdef _DEBUG_
	Serial.begin(9600);
#endif
	communication.begin();
	board.begin();
	leds.begin();
}

CellCRGB setLEDColor(uint8_t idx) {
	bool val = board.getState(idx);
	return val ? CellCRGB(0xFFFFFF) : CellCRGB(0x0);
}

void loop() {
	board.scan();
#ifdef _DEBUG_
	board.print();
#endif
	communication.tick();
	leds.showLEDs(&setLEDColor);
	// delay(1000);
}