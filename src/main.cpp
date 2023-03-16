// YDIBDGTCHB main code file
#include "constants.h"

#include <Arduino.h>

#include "senseboard.h"
#include "ledmatrix.h"
#include "wifiinterface.h"
#include "button.h"
#include "rulesengine.h"

enum class ControllerMode {
	GAME_RUNNING = 0,
	GAME_PAUSED
}; 

SenseBoard board;
LEDMatrix leds;
// WiFiManager wifiManager;

PushButton btn;

ControllerMode mode = ControllerMode::GAME_PAUSED;
ChessGameState tempState;
ClassicChessRules engine;

void setup() {
#ifdef _DEBUG_
	Serial.begin(SERIAL_BAUDRATE);
#endif
	Serial.println();
	Serial.println(tempState.toString());
	Serial.println(engine.toString());

	ChessGameState state = engine.getStartingState();
	auto moves = engine.getValidMovesForPiece(state, {1, 1});
	Serial.print(F("Generated following moves #"));
	Serial.println(moves.size());
	for (auto loc : moves) {
		Serial.println(loc.toString());
	}
	
	// if (!btn.init(PIN_PUSHBUTTON1))
	// 	return;
	board.init();
	leds.init();
	// wifiManager.init();
	mode = ControllerMode::GAME_RUNNING;
}

CellCRGB setLEDColor(uint8_t idx) {
	bool val = board.getState(idx);
	return val ? CellCRGB(0xFFFFFF) : CellCRGB(0x0);
}

void loop() {
	// if (btn.tick()) {
	// 	if (mode == ControllerMode::GAME_PAUSED) {
	// 		if (btn.held()) {
	// 			Serial.println(F("Game mode changed: PAUSED -> RUNNING"));
	// 			mode = ControllerMode::GAME_RUNNING;
	// 		}
	// 	}
	// }
	
	board.scan();
// #ifdef _DEBUG_
// 	board.print();
// #endif
	// wifiManager.tick();
	leds.showLEDs(&setLEDColor);
	delay(100);
}