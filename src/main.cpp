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
ClassicChessRules engine;

void setup() {
#ifdef _DEBUG_
	Serial.begin(SERIAL_BAUDRATE);
#endif
	Serial.println();
	Serial.print(F("Chess rules engine: "));
	Serial.println(engine.toString());

	// ChessGameState state = engine.getStartingState();
	ChessGameState state("k7/1Q6/1K6/8/8/8/8/8 b - - 0 1");

	Serial.println(state.toFEN());
	Serial.println(state.toString());


	// Serial.println(state.findFirst(CHESSPIECE::KING, state.getColorToMove()).toString());

	ChessPieceLocation possibleLocation("a8");
	Serial.print(F("=== Possible moves for "));
	Serial.print(possibleLocation.toString());
	Serial.print(F(": "));
	for (auto loc : engine.getPossibleMovesForPiece(state, possibleLocation)) {
		Serial.print(loc.toString());
		if (loc.isTaking())
			Serial.print("(x)");
		Serial.print(" ");
	}
	Serial.println();

	ChessPieceLocation validLocation("a8");
	Serial.print(F("=== Valid moves for "));
	Serial.print(validLocation.toString());
	Serial.print(F(": "));
	for (auto loc : engine.getValidMovesForPiece(state, validLocation)) {
		Serial.print(loc.toString());
		if (loc.isTaking())
			Serial.print("(x)");
		Serial.print(" ");
	}
	Serial.println();

	Serial.print("=== Is check: ");
	Serial.println(engine.isCheck(state));
	Serial.print("=== Is mate: ");
	Serial.println(engine.isMate(state));
	Serial.print("=== Is draw: ");
	Serial.println(engine.isDraw(state));
	
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
	// board.print();

	// wifiManager.tick();
	leds.showLEDs(&setLEDColor);

	delay(50);
}