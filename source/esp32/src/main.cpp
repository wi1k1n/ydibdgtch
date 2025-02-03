// YDIBDGTCHB main code file
#include <Arduino.h>

#include "application.h"

#include "rulesengine.h"

Application app;
void setup() {
	app.init();

	Serial.begin(115200);

	ChessGameState state;

	Serial.println("Hello, World!");
	Serial.println(state.ToString());
}
void loop() {
}