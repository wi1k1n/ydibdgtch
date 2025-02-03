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

		ChessGameState gameState2(String("r1bqkbnr/pp1npppp/2p5/8/3PN3/8/PPP2PPP/R1BQKBNR w KQkq - 0 1"));
		std::cout << gameState2.ToString() << std::endl << std::endl;
		gameState2.Set(1, 4, ChessPiece('Q'));
		std::cout << gameState2.ToFEN() << std::endl << std::endl;
    
    return 0;
}