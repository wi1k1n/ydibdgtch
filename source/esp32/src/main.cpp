// YDIBDGTCHB main code file
#include <Arduino.h>

#include "application.h"

#include "rulesengine.h"

Application app;
void setup() {
	app.init();

	String s;
	s.toInt();

	Serial.begin(115200);
	Serial.println("Hello, World!");

	ClassicChessRules rules;
	ChessGameState state(String("r1bqk2r/pppp1ppp/2nb1n2/1B2p3/4P3/3P1N2/PPP2PPP/RNBQK2R w KQkq - 0 1"));
	Serial.println(state.ToString());
	state.MakeMove(ChessStateMove(ChessPieceLocation(0, 4), ChessMoveLocation(0, 6, CHESSMOVEINFO::CASTLING)));
	Serial.println(state.ToString());

}
void loop() {
}