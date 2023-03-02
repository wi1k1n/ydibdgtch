#include "boardcontroller.h"

#include "commands.h"
#include "Wire.h"

bool BoardController::begin() {
	Wire.begin(PIN_CHESS_CONTROLLER_SDA, PIN_CHESS_CONTROLLER_SCL, CHESS_CONTROLLER_ADDRESS)
	return true;
}

void BoardController::tick() {
	Serial.println((int)Commands::PING);
}