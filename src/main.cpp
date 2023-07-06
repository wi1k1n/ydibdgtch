// YDIBDGTCHB main code file
#include "constants.h"
#include "sdk.h"

#include <Arduino.h>

#include <TimerMs.h>

#include "interfaces/serial.h"
#include "senseboard.h"
#include "ledmatrix.h"
#include "wifiinterface.h"
#include "button.h"
#include "rulesengine.h"
#include "stateresolver.h"

class Application {
	CommunicationProtocol<SerialCommunication> comm;

	// SenseBoardHardware board;
	SenseBoardWebGUI board;
	LEDMatrix leds;
	PushButton btn;
	// WiFiManager wifiManager;

	TimerMs tmrBoardScan { 50, 1, 0 };
	TimerMs tmrLedsUpdate { 50, 1, 0 };

	ClassicChessRules engine;
	SenseBoardStateDebouncer debouncer;
	GSResolver resolver;

	
	ChessGameState currentResolvedState; // TODO: do caching in the GSResolver instead!
public:
	bool init();
	bool tick();
};

CellCRGB setLEDColor(uint8_t idx);

bool Application::init() {
#ifdef _DEBUG_
	delay(100);
	Serial.begin(SERIAL_BAUDRATE);
	delay(100); LOGLN(); delay(100); LOGLN(); LOGLN();
#endif

	if (!comm.init() || !comm.communicationBegin())
		return false;

	Serial.print("Chess rules engine: ");
	Serial.println(engine.toString());
	
	// if (!btn.init(PIN_PUSHBUTTON1))
	// 	return false;

	if (!board.init()) 
		return false;
	if (!debouncer.init(board.getState())) 
		return false;
	
	// ChessGameState initialGameState("k1K5/8/8/6p1/2b5/3n4/1N6/8 w - - 0 1"_f);
	ChessGameState initialGameState(engine.getStartingState());
	if (!resolver.init(engine, initialGameState)) 
		return false;

	if (!leds.init()) 
		return false;
	// wifiManager.init();

#ifdef _DEBUG_
	{
		// String fen = engine.getStartingState().toFEN();
		LOGLN(initialGameState.toFEN());
		LOG(initialGameState.toString());
		// comm.send(fen);
	}
#endif
	
	return true;
}

bool Application::tick() {
	// if (comm.tick()) {

	// }
	
	if (tmrBoardScan.tick())
		board.scan();

	if (debouncer.tick(board.getState())) { // if there was a change
		board.print();
		GSResolverInfo resolveInfo = resolver.update(debouncer.getChanges());
		// LOGLN(resolveInfo.toString());
		currentResolvedState = resolver.getGameState();
		if (currentResolvedState.isUndefined()) {
			DLOGLN("Got Undefined game state!");
		} else {
			// comm.send(currentResolvedState.toFEN());
			LOGLN(currentResolvedState.toFEN());
			LOGLN(currentResolvedState.toString());
		}
		// LOG("IsUnique: "_f); LOGLN(resolver.IsCurrentStateUnique());
		// LOG("IsIntermediate: "_f); LOGLN(resolver.IsCurrentStateIntermediate());
	}

	// wifiManager.tick();
	std::function<CellCRGB(uint8_t)> setLEDColor = [&](uint8_t idx) {
		CRGB clr;
		uint8_t b = board.getState(idx) ? 255 : 0;
		CHESSCOLOR pieceColor = currentResolvedState.at(idx).getColor();
		uint8_t r = pieceColor == CHESSCOLOR::BLACK ? 255 : 0;
		uint8_t g = pieceColor == CHESSCOLOR::WHITE ? 255 : 0;
		clr.setRGB(r, g, b);
		return CellCRGB(clr);
	};
	if (tmrLedsUpdate.tick()) {
		leds.showLEDs(setLEDColor);
	}
}

Application app;
void setup() {
	if (!app.init()) {
		Serial.begin(SERIAL_BAUDRATE);
		Serial.println("Could not initialize Application class instance!"_f);
	}
}
void loop() {
	app.tick();
}