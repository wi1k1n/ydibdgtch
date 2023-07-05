// YDIBDGTCHB main code file
#include "constants.h"
#include "sdk.h"

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

// SenseBoard board;
SenseBoardSerial board;
LEDMatrix leds;
// WiFiManager wifiManager;

PushButton btn;

ControllerMode mode = ControllerMode::GAME_PAUSED;
ClassicChessRules engine;
SenseBoardStateDebouncer debouncer;
GSResolver resolver;

void setup() {
#ifdef _DEBUG_
	Serial.begin(SERIAL_BAUDRATE);
	delay(100);
#endif
	Serial.println();
	Serial.print(F("Chess rules engine: "));
	Serial.println(engine.toString());
	
	// if (!btn.init(PIN_PUSHBUTTON1))
	// 	return;
	if (!board.init()) return;
	if (!debouncer.init(board.getState())) return;
	if (!resolver.init(engine, engine.getStartingState())) return;

	if (!leds.init()) return;
	// wifiManager.init();

#ifdef _DEBUG_
	delay(20); // small delay to make sure webgui receives starting fen as a separate package
	LOG(engine.getStartingState().toFEN());
	delay(20);
	LOG(engine.getStartingState().toFEN());
	delay(20);
#endif

	mode = ControllerMode::GAME_RUNNING;
}

CellCRGB setLEDColor(uint8_t idx) {
	bool val = board.getState(idx);
	return val ? CellCRGB(0xFFFFFF) : CellCRGB(0x0);
}

void loop() {
	if (mode == ControllerMode::GAME_RUNNING) {
		board.scan();
		if (debouncer.tick(board.getState())) { // if there was a change
			// board.print();
			GSResolverInfo resolveInfo = resolver.update(debouncer.getChanges());
			if (resolveInfo.isFinished) {
				ChessGameState state = resolver.getGameState();
				if (state.isUndefined()) {
					DLOGLN("Got Undefined game state!");
				} else {
					delay(100);
					LOG(state.toFEN());
					delay(100);
				}
			}
			// LOG("IsUnique: "_f); LOGLN(resolver.IsCurrentStateUnique());
			// LOG("IsIntermediate: "_f); LOGLN(resolver.IsCurrentStateIntermediate());
		}
	}

	// wifiManager.tick();
	leds.showLEDs(&setLEDColor);

	delay(50); // TODO: timer instead, please!
}