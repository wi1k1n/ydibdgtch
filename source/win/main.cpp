#include <iostream>

#include "rulesengine.h"
#include "chs_string.h"

int main(int argc, char* argv[]) {
    std::cout << "Hello, World!" << std::endl;

		ChessGameState gameState;
		std::cout << gameState.ToString() << std::endl << std::endl;
		std::cout << gameState.ToString(false) << std::endl << std::endl;
		std::cout << gameState.ToString(true, false) << std::endl << std::endl;
		std::cout << gameState.ToString(true, true, true) << std::endl << std::endl;

		ChessGameState gameState2(String("rn1qkbnr/pp2pppp/2p5/5b2/3PN3/8/PPP2PPP/R1BQKBNR w KQkq - 0 1"));
		std::cout << gameState2.ToString() << std::endl << std::endl;
		gameState2.Set(1, 4, ChessPiece('Q'));
		std::cout << gameState2.ToFEN() << std::endl << std::endl;

		std::cout << "========================================" << std::endl;

		ClassicChessRules rules;
		ChessGameState state(String("r1bqk2r/pppp1ppp/2nb1n2/1B2p3/4P3/3P1N2/PPP2PPP/RNBQK2R w KQkq - 0 1"));
		std::cout << state.ToString() << std::endl << std::endl;
		state.MakeMove(ChessStateMove(ChessPieceLocation(0, 4), ChessMoveLocation(0, 6, CHESSMOVEINFO::CASTLING)));
		std::cout << state.ToString() << std::endl << std::endl;
    
    return 0;
}