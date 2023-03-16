#include "rulesengine.h"

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

String ChessGameState::toString() const {
	String res;
	for (uint8_t row = 0; row < 8; ++row) {
		for (uint8_t col = 0; col < 8; ++col) {
			auto entry = _pieces.find({ row, col });
			res += (entry == _pieces.end() ? "  " : entry->second.toString()) + '|';
		}
		res += '\n';
	}
	return res;
}

CHESSCOLOR ChessGameState::isLocationOccupied(const ChessPieceLocation& location) const {
	return static_cast<CHESSCOLOR>(at(location)._color);
}

ChessPiece ChessGameState::at(const ChessPieceLocation& location) const {
	auto entry = _pieces.find(location);
	return entry != _pieces.end() ? entry->second : ChessPiece();
}
ChessPiece ChessGameState::at(uint8_t row, uint8_t col) const {
	return at(ChessPieceLocation{ row, col });
}

bool ChessRulesEngine::isLocationOnBoard(const ChessPieceLocation& location) {
	return location._col >= 0 && location._col < 8 && location._row >= 0 && location._row < 8;	
}

std::vector<ChessPieceLocation> ClassicChessRules::getValidMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location) const {
	std::vector<ChessPieceLocation> moves;
	ChessPiece chessPiece = state.at(location);
	CHESSPIECE piece = static_cast<CHESSPIECE>(chessPiece._piece);
	CHESSCOLOR clr = static_cast<CHESSCOLOR>(chessPiece._color);
#ifdef _DEBUG_
		Serial.print(F("Generating moves for piece "));
		Serial.print(chessPiece.toString());
		Serial.print(F(" at location "));
		Serial.println(location.toString());
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
			int8_t dir = clr == CHESSCOLOR::WHITE ? 1 : -1;
			// single forward move
			ChessPieceLocation singleForward{ row + dir, col };
			if (isLocationOnBoard(singleForward) && state.isLocationOccupied(singleForward) == CHESSCOLOR::UNKNOWN) {
				moves.push_back(singleForward);
				// double forward move
				bool isOnInitialRow = location._row == ((7 + dir) % 7);
				ChessPieceLocation doubleForward{ row + dir * 2, col };
				if (isOnInitialRow && isLocationOnBoard(doubleForward) && state.isLocationOccupied(doubleForward) == CHESSCOLOR::UNKNOWN)
					moves.push_back(doubleForward);
			}
			// diagonal moves
			for (int8_t dx = -1; dx < 2; dx += 2) {
				ChessPieceLocation diagonal{ row + dir, col + dx };
				if (isLocationOnBoard(diagonal) && state.isLocationOccupied(diagonal) == clrOpposite)
					moves.push_back(diagonal);
			}
			break;
		}
		default:
			break;
	}

	return moves;
}