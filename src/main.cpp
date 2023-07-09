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

const String PACKETS_INCOME[] = {
	"93379838", 	// 0	setfen
	"70988515",	 	// 1	setboard
	"98716453", 	// 2	updboard
};
struct PACKETS_OUTCOME {
	const String setfen = 			"42751315";
};

const size_t SERIALBUFFERSIZE = 127;
char serialBuffer[SERIALBUFFERSIZE + 1];
void sendPacket(const String& packetOutcome, const String& msg) {
	Serial.print(packetOutcome + msg);
}
uint8_t receivePacket(String& msg) {
	if (!Serial.available()) return 255;

	uint8_t packetIdx = 255;
	size_t nBytes = Serial.readBytes(serialBuffer, 8);
	if (nBytes == 8) {
		constexpr size_t packetNumber = sizeof(PACKETS_INCOME) / sizeof(PACKETS_INCOME[0]);
		for (uint8_t i = 0; i < packetNumber; ++i) {
			if (!memcmp(serialBuffer, PACKETS_INCOME[i].begin(), 8)) {
				packetIdx = i;
				break;
			}
		}
	}

	if (packetIdx == 255) { // unknown packet type
		LOGLN(Serial.readString());
		return 255; 
	}
	
	nBytes = Serial.readBytes(serialBuffer, 4);
	if (nBytes < 4) { DLOGLN("Invalid"_f); return 255; } // invalid msg size

	char strLenBuff[5];
	memcpy(strLenBuff, serialBuffer, 4);
	strLenBuff[4] = 0;
	uint16_t strLen = atoi(strLenBuff);
	
	if (strLen < 0) { DLOGLN("Invalid"_f); return 255; } // error
	if (strLen > SERIALBUFFERSIZE) { DLOG("Wow wow, too big msg: "); LOGLN(strLen); } // warning

	msg = String();
	while (strLen > 0) {
		size_t cnt = Serial.readBytes(serialBuffer, min(SERIALBUFFERSIZE, static_cast<size_t>(strLen)));
		serialBuffer[cnt] = 0;
		msg += String(serialBuffer);
		strLen -= cnt;
	}	

	return packetIdx;
}

class Application {
	// CommunicationProtocol<SerialCommunication> comm;

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

	void communicateSerial();
};

void Application::communicateSerial() {
	String msg;
	for (uint8_t packetIdx = receivePacket(msg); packetIdx != 255; packetIdx = receivePacket(msg))
		switch (packetIdx) {
			case 0: { // setfen
				// LOG("setfen: "); LOGLN(msg);
				currentResolvedState = ChessGameState(msg);
				// for (const auto& entry : currentResolvedState._pieces) {
				// 	LOG(entry.first.toString());
				// 	LOG(" => "_f);
				// 	LOGLN(entry.second.toString());
				// }
				LOGLN("New resolved state: "_f);
				LOGLN(currentResolvedState.toString());
				if (!resolver.init(engine, ChessGameState(msg))) 
					DLOGLN("Couldn't init resolver!"_f);
				currentResolvedState = resolver.getGameState();
				break;
			}
			case 1: { // setboard
				// LOG("setboard: "); LOGLN(msg);
				if (msg.length() != 64) {
					DLOGLN("Incorrect board state!"_f);
				} else {
					for (uint8_t i = 0; i < 64; ++i)
						board.setState(i, msg[i] == '1');
					if (!debouncer.init(board.getState())) 
						DLOGLN("Couldn't initialize debouncer!"_f);
				}
				break;
			}
			case 2: { // updboard
				// LOG("updboard: "); LOGLN(msg);
				if (msg.length() != 64) {
					DLOGLN("Incorrect board state!"_f);
				} else {
					for (uint8_t i = 0; i < 64; ++i)
						board.setState(i, msg[i] == '1');
				}
				break;
			}
		}
}

bool Application::init() {
#ifdef _DEBUG_
	delay(100);
	Serial.begin(SERIAL_BAUDRATE);
	delay(100); LOGLN(); delay(100); LOGLN(); LOGLN();
#endif

	// if (!comm.init() || !comm.communicationBegin())
	// 	return false;

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
	communicateSerial();
	if (tmrBoardScan.tick())
		board.scan();

	if (debouncer.tick(board.getState())) { // if there was a change
		board.print();
		bool success = resolver.update(debouncer.getChanges());
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