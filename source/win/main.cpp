#include <iostream>

#include "rulesengine.h"

int main(int argc, char* argv[]) {
	std::cout << "Hello, World!" << std::endl;

	ChessPiece piece('Q');
	if (piece.IsValid())
		std::cout << "Piece is valid" << std::endl;
	else
		std::cout << "Piece is invalid" << std::endl;
	
	return 0;
}