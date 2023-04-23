// YDIBDGTCHB main code file
#include "constants.h"

#include <Arduino.h>

#include "senseboard.h"
#include "ledmatrix.h"
#include "wifiinterface.h"
#include "button.h"
#include "rulesengine.h"
#include "stateresolver.h"

enum class ControllerMode {
	GAME_RUNNING = 0,
	GAME_PAUSED
}; 

SenseBoard board;
LEDMatrix leds;
// WiFiManager wifiManager;

PushButton btn;

ControllerMode mode = ControllerMode::GAME_PAUSED;
ClassicChessRules engine;
SenseBoardStateDebouncer debouncer;
ChessGameStatesResolver resolver;

void setup() {
#ifdef _DEBUG_
	Serial.begin(SERIAL_BAUDRATE);
#endif
	Serial.println();
	Serial.print(F("Chess rules engine: "));
	Serial.println(engine.toString());
	
	// if (!btn.init(PIN_PUSHBUTTON1))
	// 	return;
	if (!board.init()) return;
	if (!resolver.init(engine)) return;
	if (!leds.init()) return;
	// wifiManager.init();

	mode = ControllerMode::GAME_RUNNING;
}

CellCRGB setLEDColor(uint8_t idx) {
	bool val = board.getState(idx);
	return val ? CellCRGB(0xFFFFFF) : CellCRGB(0x0);
}

void loop() {
	if (mode == ControllerMode::GAME_RUNNING) {
		board.scan();
		SenseBoardState curState = board.getState();
		if (debouncer.tick(curState)) {
			resolver.update(curState - debouncer.getPrev());
		}
	}
	// board.print();

	// wifiManager.tick();
	leds.showLEDs(&setLEDColor);

	delay(50);
}