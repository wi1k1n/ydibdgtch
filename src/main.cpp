// YDIBDGTCHB main code file

#include "constants.h"

#include <Arduino.h>

#include "senseboard.h"
#include "ledmatrix.h"
#include "wifiinterface.h"

SenseBoard board;
LEDMatrix leds;
WiFiManager wifiManager;

void setup() {
#ifdef _DEBUG_
	Serial.begin(SERIAL_BAUDRATE);
#endif
	board.begin();
	leds.begin();
	wifiManager.begin();
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
	wifiManager.tick();
	leds.showLEDs(&setLEDColor);
	// delay(1000);
}