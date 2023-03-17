#include "rulesengine.h"

#include <vector>
#include <algorithm>
#include <utility>

char ChessPiece::_pieceSymbols[] = { 'p', 'n', 'b', 'r', 'q', 'k' };

ChessPiece::ChessPiece(const String& s) 
	: _piece(static_cast<uint8_t>(CHESSPIECE::UNKNOWN)), _color(static_cast<uint8_t>(CHESSCOLOR::UNKNOWN)), _history(0) {
#ifdef _CHRULESDBG_
	Serial.print(F("Constructing chess piece from string: "));
	Serial.println(s);
#endif

	if (s.isEmpty())
		return;
	
	char whiteSymbol = s[0];
	if (whiteSymbol < 'a')
		whiteSymbol += 'a' - 'A';

	uint8_t pieceSymbolsCount = countof(_pieceSymbols);
	for (uint8_t i = 0; i < pieceSymbolsCount; ++i) {
		if (whiteSymbol == _pieceSymbols[i]) {
			_piece = i;
			_color = static_cast<uint8_t>(s[0] == whiteSymbol ? CHESSCOLOR::BLACK : CHESSCOLOR::WHITE);
			break;
		}
	}

	if (s.length() > 1)
		_history = s[1] == '.';

#ifdef _CHRULESDBG_
	Serial.print(F("   success: "));
	Serial.println(toString());
#endif
}

String ChessPiece::toString(bool symbolic) const {
	String snumeric = String(_color) + "-" + String(_piece) + "." + String(_history);
	String ssymbolic;
	if (_piece >= countof(_pieceSymbols) || getColor() == CHESSCOLOR::UNKNOWN)
		ssymbolic = String('?');
	else
		ssymbolic = String(getColor() == CHESSCOLOR::BLACK ? _pieceSymbols[_piece] : (static_cast<char>(_pieceSymbols[_piece] - ('a' - 'A'))));

// #ifdef _CHRULESDBG_
// 	Serial.print(ssymbolic); Serial.print(" <- "); Serial.println(snumeric);
// #endif
	if (!symbolic)
		return snumeric;
	return ssymbolic;
}

ChessPieceLocation::ChessPieceLocation(const String& s)
	: _row(-1), _col(-1) {
#ifdef _CHRULESDBG_
	Serial.print(F("Constructing location from string: "));
	Serial.println(s);
#endif

	if (s.length() != 2)
		return;
	
	const char row = s[1];
	char col = s[0];
	if (col < 'a')
		col += 'a' - 'A';
	_row = (row < '1' || row > '8') ? -1 : (row - '1');
	_col = (col < 'a' || col > 'h') ? -1 : (col - 'a');

#ifdef _CHRULESDBG_
	Serial.print(F("   success: "));
	Serial.println(toString());
#endif
}

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
	: _pieces(other._pieces), _colorToMove(other._colorToMove) { }

ChessGameState::ChessGameState(const String& fenString) {
	Serial.println(F("GameState from FEN string is not implemented yet!"));
}

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
			res += " " + String(static_cast<char>('a' + col)) + " " + (col == 7 ? "" : " ");
		res += '\n';
		if (!transpose)
			res += '\n';
	};

	if (legend && !transpose)
		drawColLegend();

	const int8_t rowStart = 7 * transpose + 0; // transpose ? 7 : 0;
	const int8_t rowEnd = -9 * transpose + 8; // transpose ? -1 : 8;
	const int8_t rowIncrement = -2 * transpose + 1; // transpose ? -1 : 1;
	for (int8_t row = rowStart; row != rowEnd; row += rowIncrement) {
		if (legend)
			res += String(row + (!zeroBased)) + "   ";
		for (uint8_t col = 0; col < 8; ++col) {
			auto entry = _pieces.find({ static_cast<uint8_t>(row), col });
			res += " " + (entry == _pieces.end() ? " " : entry->second.toString()) + " " + (col == 7 ? "" : "|");
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
// #ifdef _CHRULESDBG_
// 	Serial.print(F("at: "));
// 	Serial.print(location.toString());
// 	Serial.print(F(" = "));
// 	Serial.println(piece.toString());
// #endif
	return piece;
}
ChessPiece ChessGameState::at(uint8_t row, uint8_t col) const {
	return at(ChessPieceLocation{ row, col });
}
ChessPiece ChessGameState::at(const String& s) const {
	return at(ChessPieceLocation(s));
}
void ChessGameState::set(const ChessPieceLocation& location, const ChessPiece& piece) {
	if (!location.isOnBoard())
		return;
	if (piece.isValid())
		_pieces.insert(std::make_pair(location, piece));
	else
		_pieces.erase(location);

}
void ChessGameState::set(uint8_t row, uint8_t col, const ChessPiece& piece) {
	set(ChessPieceLocation{ row, col }, piece);
}
void ChessGameState::set(const String& location, const ChessPiece& piece) {
	set(ChessPieceLocation(location), piece);
}
void ChessGameState::set(const String& location, const String& piece) {
	set(location, ChessPiece(piece));
}

std::vector<ChessPieceLocation> ClassicChessRules::getValidMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location) const {
	std::vector<ChessPieceLocation> moves;
	ChessPiece chessPiece = state.at(location);
	CHESSPIECE piece = static_cast<CHESSPIECE>(chessPiece._piece);
	CHESSCOLOR clr = static_cast<CHESSCOLOR>(chessPiece._color);
#ifdef _CHRULESDBG_
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
#ifdef _CHRULESDBG_
			Serial.println(F("> Pawn"));
#endif
			int8_t dir = clr == CHESSCOLOR::WHITE ? 1 : -1;
			// single forward move
			ChessPieceLocation singleForward{ static_cast<int8_t>(row) + dir, col };
			if (singleForward.isOnBoard() && state.at(singleForward).getColor() == CHESSCOLOR::UNKNOWN) {
				moves.push_back(singleForward);
#ifdef _CHRULESDBG_
				Serial.println(F(">>> single forward"));
#endif
				// double forward move
				bool isOnInitialRow = location._row == ((7 + dir) % 7); // 1 for white, 6 for black
				ChessPieceLocation doubleForward{ static_cast<int8_t>(row) + dir * 2, col };
				if (isOnInitialRow && doubleForward.isOnBoard() && state.at(doubleForward).getColor() == CHESSCOLOR::UNKNOWN) {
					moves.push_back(doubleForward);
#ifdef _CHRULESDBG_
					Serial.println(F(">>> double forward"));
#endif
				}
			}
			// diagonal moves
			bool isOnEnPassantRow = location._row == ((dir + 1) / 2 + 3); // 4 for white, 3 for black
			for (int8_t dx = -1; dx < 2; dx += 2) {
				// ordinary diagonal
				ChessPieceLocation diagonal{ static_cast<int8_t>(row) + dir, col + dx };
				if (diagonal.isOnBoard() && state.at(diagonal).getColor() == clrOpposite) {
					moves.push_back(diagonal);
#ifdef _CHRULESDBG_
					Serial.print(F(">>> diagonal "));
					Serial.println(dir > 0 ? F("right") : F("left"));
#endif
				}
				// en-passant
				if (!isOnEnPassantRow)
					continue;
				ChessPieceLocation diagEnPassant{ static_cast<int8_t>(row), col + dx };
				if (diagEnPassant.isOnBoard()) {
					ChessPiece pieceOpp = state.at(diagEnPassant);
					if (pieceOpp.getColor() == clrOpposite && pieceOpp.getPiece() == CHESSPIECE::PAWN && pieceOpp.getHistory()) {
						moves.push_back(diagEnPassant);
#ifdef _CHRULESDBG_
						Serial.print(F(">>> en-passant "));
						Serial.println(dir > 0 ? F("right") : F("left"));
#endif
					}
				}
			}
			break;
		}
		case CHESSPIECE::KNIGHT: {
#ifdef _CHRULESDBG_
			Serial.println(F("> Knight"));
#endif
			std::vector<std::tuple<int8_t, int8_t>> dirs;
			cartesianProduct({ -1, 1 }, { -2, 2 }, back_inserter(dirs));

			for (auto&& pos : dirs) {
				int8_t first = std::get<0>(pos);
				int8_t second = std::get<1>(pos);
				std::vector<std::pair<int8_t, int8_t>> locs = {
					std::make_pair(row + first, col + second),
					std::make_pair(row + second, col + first)
				};
				for (int8_t flip = 0; flip < 2; ++flip) {
					std::pair<int8_t, int8_t> locPair = locs[flip];
					if (!ChessPieceLocation::isOnBoard(locPair.first, locPair.second))
						continue;
					ChessPieceLocation loc(locPair);
					if (loc.isOnBoard() && state.at(loc).getColor() != clr) {
						moves.push_back(loc);
#ifdef _CHRULESDBG_
						Serial.print(F(">>> "));
						Serial.println(loc.toString());
#endif
					}
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