// YDIBDGTCHB main code file
#include "constants.h"
#include "sdk.h"

#include <Arduino.h>

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
	// WiFiManager wifiManager;

	PushButton btn;

	ClassicChessRules engine;
	SenseBoardStateDebouncer debouncer;
	GSResolver resolver;
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
	if (!resolver.init(engine, engine.getStartingState())) 
		return false;

	if (!leds.init()) 
		return false;
	// wifiManager.init();

#ifdef _DEBUG_
	{
		String fen = engine.getStartingState().toFEN();
		// LOG(fen);
		comm.send(fen);
	}
#endif
	return true;
}

bool Application::tick() {
	if (comm.tick()) {

	}
	board.scan();
	if (debouncer.tick(board.getState())) { // if there was a change
		// board.print();
		GSResolverInfo resolveInfo = resolver.update(debouncer.getChanges());
		LOGLN(resolveInfo.toString());
		ChessGameState state = resolver.getGameState();
		if (state.isUndefined()) {
			DLOGLN("Got Undefined game state!");
		} else {
			comm.send(state.toFEN());
		}
		// LOG("IsUnique: "_f); LOGLN(resolver.IsCurrentStateUnique());
		// LOG("IsIntermediate: "_f); LOGLN(resolver.IsCurrentStateIntermediate());
	}

	// wifiManager.tick();
	std::function<CellCRGB(uint8_t)> setLEDColor = [&](uint8_t idx) {
		bool val = board.getState(idx);
		return val ? CellCRGB(0xFFFFFF) : CellCRGB(0x0);
	};
	leds.showLEDs(setLEDColor);

	delay(50); // TODO: timer instead, please!
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