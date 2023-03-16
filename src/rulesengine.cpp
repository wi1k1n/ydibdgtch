#include "rulesengine.h"
#include <algorithm>

char ChessPiece::_pieceSymbols[] = { 'p', 'N', 'B', 'R', 'Q', 'K' };

ChessGameState::ChessGameState(const CHESSINITIALSTATE& initState, const CHESSCOLOR& colorToMove) {
	_colorToMove = colorToMove;
	if (initState == CHESSINITIALSTATE::CLASSIC) {
		_pieces.reserve(16);
		std::initializer_list<CHESSPIECE> startLine = {
			CHESSPIECE::ROOK,
			CHESSPIECE::KNIGHT,
			CHESSPIECE::BISHOP,
			CHESSPIECE::QUEEN,
			CHESSPIECE::KING,
			CHESSPIECE::BISHOP,
			CHESSPIECE::KNIGHT,
			CHESSPIECE::ROOK
		};
		_fillRow(0, startLine, CHESSCOLOR::WHITE);
		_fillRow(1, CHESSPIECE::PAWN, CHESSCOLOR::WHITE);
		_fillRow(6, CHESSPIECE::PAWN, CHESSCOLOR::BLACK);
		_fillRow(7, startLine, CHESSCOLOR::BLACK);
	}
}

ChessGameState::ChessGameState(const ChessGameState& other)
	: _pieces(other._pieces), _colorToMove(other._colorToMove) {}

void ChessGameState::_fillRow(uint8_t row, CHESSPIECE piece, CHESSCOLOR color) {
	for (uint8_t i = 0; i < 8; ++i)
		_pieces.emplace(std::make_pair(ChessPieceLocation{ row, i }, ChessPiece{ piece, color }));
}
void ChessGameState::_fillRow(uint8_t row, const std::initializer_list<CHESSPIECE>& pieces, CHESSCOLOR color) {
	auto it = pieces.begin();
	for (uint8_t i = 0; i < 8 && it != pieces.end(); ++i, ++it)
		_pieces.emplace(std::make_pair(ChessPieceLocation{ row, i }, ChessPiece{ *it, color }));
}
void ChessGameState::_fillCol(uint8_t col, CHESSPIECE piece, CHESSCOLOR color) {
	for (uint8_t i = 0; i < 8; ++i)
		_pieces.emplace(std::make_pair(ChessPieceLocation{ i, col }, ChessPiece{ piece, color }));
}
void ChessGameState::_fillCol(uint8_t col, const std::initializer_list<CHESSPIECE>& pieces, CHESSCOLOR color) {
	auto it = pieces.begin();
	for (uint8_t i = 0; i < 8 && it != pieces.end(); ++i, ++it)
		_pieces.emplace(std::make_pair(ChessPieceLocation{ i, col }, ChessPiece{ *it, color }));
}

String ChessGameState::toString(bool legend, bool transpose, bool zeroBased) const {
	String res;
	auto drawColLegend = [&res, legend, transpose]() {
		res += String(transpose ? "\n" : "") + String(legend ? "    " : "");
		for (uint8_t col = 0; col < 8; ++col)
			res += String(static_cast<char>('a' + col)) + " " + (col == 7 ? "" : " ");
		res += '\n';
		if (!transpose)
			res += '\n';
	};

	if (legend && !transpose)
		drawColLegend();

	int8_t rowStart = 7 * transpose + 0; // transpose ? 7 : 0;
	int8_t rowEnd = -9 * transpose + 8; // transpose ? -1 : 8;
	int8_t rowIncrement = -2 * transpose + 1; // transpose ? -1 : 1;
	for (int8_t row = rowStart; row != rowEnd; row += rowIncrement) {
		if (legend)
			res += String(row + (!zeroBased)) + "   ";
		for (uint8_t col = 0; col < 8; ++col) {
			auto entry = _pieces.find({ static_cast<uint8_t>(row), col });
			res += (entry == _pieces.end() ? "  " : entry->second.toString()) + (col == 7 ? "" : "|");
		}
		res += '\n';
	}

	if (legend && transpose)
		drawColLegend();
	
	return res;
}

bool ChessGameState::isLocationOccupied(const ChessPieceLocation& location) const {
	return static_cast<CHESSCOLOR>(at(location)._color) != CHESSCOLOR::UNKNOWN;
}

ChessPiece ChessGameState::at(const ChessPieceLocation& location) const {
	auto entry = _pieces.find(location);
	ChessPiece piece = entry != _pieces.end() ? entry->second : ChessPiece();
#ifdef _CHRULESDBG_
	Serial.print(F("at: "));
	Serial.print(location.toString(0));
	Serial.print(F(" = "));
	Serial.println(piece.toString(0));
#endif
	return piece;
}
ChessPiece ChessGameState::at(uint8_t row, uint8_t col) const {
	return at(ChessPieceLocation{ row, col });
}
void ChessGameState::set(const ChessPieceLocation& location, const ChessPiece& piece) {
	_pieces.insert(std::make_pair(location, piece));
}
void ChessGameState::set(uint8_t row, uint8_t col, const ChessPiece& piece) {
	set(ChessPieceLocation{ row, col }, piece);
}

bool ChessRulesEngine::isLocationOnBoard(const ChessPieceLocation& location) {
	bool ret = location._col >= 0 && location._col < 8 && location._row >= 0 && location._row < 8;
#ifdef _CHRULESDBG_
	Serial.print(F("isLocationOnBoard: "));
	Serial.print(location.toString(0));
	Serial.print(F(" = "));
	Serial.println(ret);
#endif
	return ret;
}

std::vector<ChessPieceLocation> ClassicChessRules::getValidMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location) const {
	std::vector<ChessPieceLocation> moves;
	ChessPiece chessPiece = state.at(location);
	CHESSPIECE piece = static_cast<CHESSPIECE>(chessPiece._piece);
	CHESSCOLOR clr = static_cast<CHESSCOLOR>(chessPiece._color);
#ifdef _CHRULESDBG_
		Serial.print(F("Generating moves for piece "));
		Serial.print(chessPiece.toString(0));
		Serial.print(F(" at location "));
		Serial.println(location.toString(0));
#endif
	if (piece == CHESSPIECE::UNKNOWN || clr == CHESSCOLOR::UNKNOWN) {
#ifdef _DEBUG_
		Serial.println(F("No piece at given location"));
#endif
		return moves;
	}
	uint8_t row = location._row;
	uint8_t col = location._col;
	CHESSCOLOR clrOpposite = clr == CHESSCOLOR::WHITE ? CHESSCOLOR::BLACK : CHESSCOLOR::WHITE;

	switch (piece) {
		case CHESSPIECE::PAWN: {
#ifdef _CHRULESDBG_
			Serial.println(F("> Pawn"));
#endif
			int8_t dir = clr == CHESSCOLOR::WHITE ? 1 : -1;
			// single forward move
			ChessPieceLocation singleForward{ static_cast<int8_t>(row) + dir, col };
			if (isLocationOnBoard(singleForward) && state.at(singleForward).getColor() == CHESSCOLOR::UNKNOWN) {
				moves.push_back(singleForward);
#ifdef _CHRULESDBG_
				Serial.println(F(">>> single forward"));
#endif
				// double forward move
				bool isOnInitialRow = location._row == ((7 + dir) % 7);
				ChessPieceLocation doubleForward{ static_cast<int8_t>(row) + dir * 2, col };
				if (isOnInitialRow && isLocationOnBoard(doubleForward) && state.at(doubleForward).getColor() == CHESSCOLOR::UNKNOWN) {
					moves.push_back(doubleForward);
#ifdef _CHRULESDBG_
					Serial.println(F(">>> double forward"));
#endif
				}
			}
			// diagonal moves
			for (int8_t dx = -1; dx < 2; dx += 2) {
				ChessPieceLocation diagonal{ static_cast<int8_t>(row) + dir, col + dx };
				if (isLocationOnBoard(diagonal) && state.at(diagonal).getColor() == clrOpposite) {
					moves.push_back(diagonal);
#ifdef _CHRULESDBG_
					Serial.println(F(">>> diagonal"));
#endif
				}
			}
			break;
		}
		default:
			Serial.println(F("> default"));
			break;
	}

	return moves;
}