#include "rulesengine.h"

#include <vector>
#include <algorithm>
#include <utility>
#include <unordered_map>
#include <unordered_set>

char ChessPiece::_pieceSymbols[] = { 'p', 'n', 'b', 'r', 'q', 'k' };

ChessPiece::ChessPiece(const String& s) 
	: _piece(static_cast<uint8_t>(CHESSPIECE::UNKNOWN)), _color(static_cast<uint8_t>(CHESSCOLOR::UNKNOWN)), _history(0) {
// #ifdef _CHRULESDBG_
// 	Serial.print(F("Constructing chess piece from string: "));
// 	Serial.println(s);
// #endif

	if (s.isEmpty())
		return;
	
	_initFromChar(s[0]);

	if (s.length() > 1)
		_history = s[1] == '.';

// #ifdef _CHRULESDBG_
// 	Serial.print(F("   success: "));
// 	Serial.println(toString());
// #endif
}

ChessPiece::ChessPiece(char c)
	: _piece(static_cast<uint8_t>(CHESSPIECE::UNKNOWN)), _color(static_cast<uint8_t>(CHESSCOLOR::UNKNOWN)), _history(0) {
	_initFromChar(c);
}

void ChessPiece::_initFromChar(char c) {
	char whiteSymbol = c;
	if (whiteSymbol < 'a')
		whiteSymbol += 'a' - 'A';

	uint8_t pieceSymbolsCount = countof(_pieceSymbols);
	for (uint8_t i = 0; i < pieceSymbolsCount; ++i) {
		if (whiteSymbol == _pieceSymbols[i]) {
			_piece = i;
			_color = static_cast<uint8_t>(c == whiteSymbol ? CHESSCOLOR::BLACK : CHESSCOLOR::WHITE);
			break;
		}
	}
}

String ChessPiece::toString(bool symbolic) const {
	if (!symbolic)
		return String(_color) + "-" + String(_piece) + "." + String(_history);
	if (_piece >= countof(_pieceSymbols) || getColor() == CHESSCOLOR::UNKNOWN)
		return String('?');
	return String(getColor() == CHESSCOLOR::BLACK ? _pieceSymbols[_piece] : (static_cast<char>(_pieceSymbols[_piece] - ('a' - 'A'))));
}

ChessPieceLocation::ChessPieceLocation(const String& s)
	: _row(-1), _col(-1) {
// #ifdef _CHRULESDBG_
// 	Serial.print(F("Constructing location from string: "));
// 	Serial.println(s);
// #endif

	if (s.length() != 2)
		return;
	
	const char row = s[1];
	char col = s[0];
	if (col < 'a')
		col += 'a' - 'A';
	_row = (row < '1' || row > '8') ? -1 : (row - '1');
	_col = (col < 'a' || col > 'h') ? -1 : (col - 'a');

// #ifdef _CHRULESDBG_
// 	Serial.print(F("   success: "));
// 	Serial.println(toString());
// #endif
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
	: _pieces(other._pieces), _colorToMove(other._colorToMove), _fullMoves(other._fullMoves), _halfMoves(other._halfMoves) {
	// for (const auto& entry : other._pieces) {
	// 	LOG(entry.first.toString());
	// 	LOG(" => "_f);
	// 	LOGLN(entry.second.toString());
	// }
	// for (const auto& entry : _pieces) {
	// 	LOG(entry.first.toString());
	// 	LOG(" => "_f);
	// 	LOGLN(entry.second.toString());
	// }
}

ChessGameState::ChessGameState(const String& fenString, bool allowPartial) {
	_initFromFEN(fenString, allowPartial);
}

bool ChessGameState::_initFromFEN(const String& fenString, bool allowPartial) {
	LOG("Loading from FEN: ");
	LOGLN(fenString);

	bool minimallyInitialized = false;
	auto invalidate = [this, allowPartial, minimallyInitialized]() {
		
		// for (const auto& entry : _pieces) {
		// 	LOG(entry.first.toString());
		// 	LOG(" => "_f);
		// 	LOGLN(entry.second.toString());
		// }
		if (allowPartial && minimallyInitialized)
			return true;
		this->_pieces.clear();
		this->_colorToMove = CHESSCOLOR::UNKNOWN;
		this->_fullMoves = 0;
		this->_halfMoves = 1;
		return false;
	};

	// 1. Handle pieces placement part
	uint8_t fenCursor = 0;
	std::vector<String> rows(8);
	{ // retrieve strings that corresponds to rows
		uint8_t rowIdx = 0;
		for (fenCursor = 0; fenCursor < fenString.length(); ++fenCursor) {
			char cc = fenString[fenCursor];
			if (cc == ' ')
				break;
			if (cc == '/') {
				++rowIdx;
				continue;
			}
			rows[rowIdx] += fenString[fenCursor];
		}
		// Validate content and create pieces
		if (rowIdx != 7)
			return invalidate();
	}
	
	// process rows
	for (int8_t rowIdx = 7; rowIdx >= 0; --rowIdx) {
		const String& row = rows[7 - rowIdx];

		uint8_t pieceCounter = 0;
		for (uint8_t i = 0; i < row.length(); ++i) {
			char cc = row[i];
			if (cc >= '1' && cc <= '8') {
				uint8_t n = cc - '0';
				if (pieceCounter + n > 8)
					return invalidate();
				pieceCounter += n;
				continue;
			}
			if (pieceCounter > 7)
				return invalidate();
			ChessPiece piece(cc);
			if (!piece.isValid())
				return invalidate();
			set(rowIdx, pieceCounter++, piece);
			LOG(at(rowIdx, pieceCounter-1).toString()); LOG(" -> "_f); LOG(rowIdx); LOG(";"); LOGLN(pieceCounter - 1);
		}
		if (pieceCounter > 8)
			return invalidate();
	}
	minimallyInitialized = true;
	LOGLN("1. Handle pieces placement part");

	// 2. Color to move
	if (++fenCursor >= fenString.length())
		return invalidate();
	
	char clrToMoveChar = fenString[fenCursor++];
	if (clrToMoveChar == 'w')
		_colorToMove = CHESSCOLOR::WHITE;
	else if (clrToMoveChar == 'b')
		_colorToMove = CHESSCOLOR::BLACK;
	else
		return invalidate();
	
	if (fenCursor >= fenString.length() || fenString[fenCursor++] != ' ')
		return invalidate();
	LOGLN("2. Color to move");
	
	// 3. Castling options
	if (fenCursor >= fenString.length())
		return invalidate();
	char castlingCharFirst = fenString[fenCursor++];
	if (castlingCharFirst != '-') {
		if (castlingCharFirst == ' ')
			return invalidate();
		// TODO: implement castling incl. Shredder-FEN standard
		uint8_t castlingCharCount = 1;
		for (; fenCursor < fenString.length(); ++fenCursor, ++castlingCharCount)
			if (fenString[fenCursor] == ' ')
				break;
		if (castlingCharCount > 4)
			return invalidate();
	}
	LOGLN("3. Castling options");
	
	// 4. En-passant pawn
	if (++fenCursor >= fenString.length())
		return invalidate();

	char epCharFirst = fenString[fenCursor++];
	if (epCharFirst != '-') {
		if (epCharFirst < 'a')
			epCharFirst += 'a' - 'A';
		if (epCharFirst < 'a' || epCharFirst > 'h')
			return invalidate();
		// get en-passant sqare row
		if (fenCursor >= fenString.length())
			return invalidate();
		char epCharRow = fenString[fenCursor++];
		// get pawn sucseptible to en-passant take
		int8_t pawnRow = -1;
		if (_colorToMove == CHESSCOLOR::BLACK && epCharRow == '3')
			pawnRow = 4;
		else if (_colorToMove == CHESSCOLOR::WHITE && epCharRow == '6')
			pawnRow = 5;
		if (pawnRow < 0)
			return invalidate();
		ChessPiece pawn = at(pawnRow, epCharFirst - 'a');
		if (!pawn.isValid() || pawn.getPiece() != CHESSPIECE::PAWN || pawn.getColor() == _colorToMove)
			invalidate();
		pawn.setHistory(true);
	}
	LOGLN("4. En-passant pawn");
	
	// 5. Half-moves since last pawn advance
	if (fenCursor >= fenString.length() || fenString[fenCursor++] != ' ')
		return invalidate();
	
	String halfMoves;
	for (; fenCursor < fenString.length(); ++fenCursor) {
		char cc = fenString[fenCursor];
		if (cc == ' ')
			break;
		if (!isDigit(cc))
			return invalidate();
		halfMoves += static_cast<char>(cc);
	}
	_halfMoves = halfMoves.toInt();
	LOGLN("5. Half-moves since last pawn advance");
	
	// 6. Full-moves since start
	if (fenCursor >= fenString.length() || fenString[fenCursor++] != ' ')
		return invalidate();
	
	String fullMoves;
	for (; fenCursor < fenString.length(); ++fenCursor) {
		char cc = fenString[fenCursor];
		if (cc == ' ')
			break;
		if (!isDigit(cc))
			return invalidate();
		fullMoves += static_cast<char>(cc);
	}
	_fullMoves = fullMoves.toInt();
	if (_fullMoves == 0)
		return invalidate();
	LOGLN("6. Full-moves since start");
	
	return true;
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
String ChessGameState::toFEN() const {
	String fen;
	uint8_t emptyCounter = 0;
	for (int8_t row = 7; row >= 0; --row) {
		// Serial.println(row);
		for (uint8_t col = 0; col < 8; ++col) {
			// Serial.print(col);
			ChessPiece piece = at(row, col);
			if (!piece.isValid()) {
				++emptyCounter;
				continue;
			}
			if (emptyCounter) {
				fen += String(static_cast<int>(emptyCounter));
				emptyCounter = 0;
			}
			fen += piece.toString();
		}
		if (emptyCounter) {
			fen += String(static_cast<int>(emptyCounter));
			emptyCounter = 0;
		}
		fen += row > 0 ? "/" : "";
		// Serial.println();
	}
	fen += String(" ") + (_colorToMove == CHESSCOLOR::BLACK ? "b" : "w");
	fen += " KQkq"; // TODO: handle castling properly!

	// semantically only a single en-passant pawn exists -> first occurence
	String enPassant("-");
	for (auto& entry : _pieces) {
		ChessPiece piece = entry.second;
		if (piece.getPiece() != CHESSPIECE::PAWN)
			continue;
		if (piece.getHistory()) {
			enPassant = entry.first.toString();
			break;
		}
	}
	fen += " " + enPassant;
	fen += " " + String(_fullMoves) + " " + String(_halfMoves);
	return fen;
}

bool ChessGameState::isLocationOccupied(const ChessPieceLocation& location) const {
	return static_cast<CHESSCOLOR>(at(location)._color) != CHESSCOLOR::UNKNOWN;
}

ChessPiece ChessGameState::at(const ChessPieceLocation& location) const {
	auto entry = _pieces.find(location);
	return entry != _pieces.end() ? entry->second : ChessPiece();
}
ChessPiece ChessGameState::at(uint8_t row, uint8_t col) const {
	return at(ChessPieceLocation{ row, col });
}
ChessPiece ChessGameState::at(uint8_t idx) const {
	return at(idx / 8, idx % 8);
}
ChessPiece ChessGameState::at(const String& s) const {
	return at(ChessPieceLocation(s));
}
void ChessGameState::set(const ChessPieceLocation& location, const ChessPiece& piece) {
	// LOGLN(location.isOnBoard() + String(" ") + piece.isValid());
	if (!location.isOnBoard())
		return;
	if (piece.isValid())
		_pieces[location] = piece;
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
void ChessGameState::unset(const ChessPieceLocation& location) {
	set(location, ChessPiece());
}
void ChessGameState::unset(uint8_t row, uint8_t col) {
	set(row, col, ChessPiece());
}
void ChessGameState::unset(const String& location) {
	set(location, ChessPiece());
}

bool ChessGameState::makeMove(const ChessMove& move) {
	// TODO: this implementation is not tested!
	if (move.first == move.second)
		return true;
	ChessPiece from = at(move.first);
	if (!from.isValid())
		return false;
	set(move.second, from);
	unset(move.first);
	return true;
}

ChessPieceLocation ChessGameState::findFirst(CHESSPIECE piece, CHESSCOLOR color) const {
	for (const auto& entry : _pieces)
		if (entry.second.getPiece() == piece && entry.second.getColor() == color)
			return entry.first;
	return ChessPieceLocation();
}
ChessPieceLocation ChessGameState::findFirst(const ChessPiece& piece) const {
	return findFirst(piece.getPiece(), piece.getColor());
}

bool ClassicChessRules::isCheck(const ChessGameState& state, CHESSCOLOR color) const {
	if (color == CHESSCOLOR::UNKNOWN)
		color = state.getColorToMove();
	// DLOGLN(String("isCheck (") + (color == CHESSCOLOR::BLACK ? "b" : (color == CHESSCOLOR::WHITE ? "w" : "?")) + String("): "));
	// LOGLN(state.toString());

	// TODO: optimize??
	ChessPieceLocation locKing = state.findFirst(CHESSPIECE::KING, color);
	if (!locKing.isOnBoard()) {
		// DLOGLN("FALSE");
		return false;
	}
	for (const auto& entry : state.getPieces()) {
		ChessPieceLocation loc = entry.first;
		ChessPiece piece = entry.second;
		if (!loc.isOnBoard() || !piece.isValid() || piece.getColorOpposite() != color) // only iterate over opponent's pieces
			continue;
		std::vector<ChessMoveLocation> takeMoves = getPossibleMovesForPiece(state, loc, true);
		// LOG(loc.toString() + "(" + piece.toString() + ")" + ">" + (takeMoves.empty() ? "." : ""));
		// for (auto& move : takeMoves)
		// 	LOG(move.toString() + " ");
		// LOG(";");
		for (const auto& move : takeMoves)
			if (static_cast<ChessMoveLocation>(move) == locKing) {
				// DLOGLN("TRUE");
				return true;
			}
	}
	// LOGLN();
	return false;
}

bool ClassicChessRules::isMate(const ChessGameState& state, CHESSCOLOR color) const {
	if (!isCheck(state, color)) {
		// Serial.println("Not in check!");
		return false;
	}
	
	if (color == CHESSCOLOR::UNKNOWN)
		color = state.getColorToMove();
	// DLOGLN(String("isMate (") + (color == CHESSCOLOR::BLACK ? "b" : (color == CHESSCOLOR::WHITE ? "w" : "?")) + String("): "));

	// TODO: optimize??
	for (const auto& entry : state.getPieces()) {
		ChessPieceLocation loc = entry.first;
		ChessPiece piece = entry.second;
		if (!loc.isOnBoard() || !piece.isValid() || piece.getColor() != color) // only iterate over our pieces
			continue;
		auto validMoves = getValidMovesForPiece(state, loc);
		// LOG(loc.toString() + ">" + (validMoves.empty() ? "." : ""));
		// for (auto& move : validMoves)
		// 	LOG(move.toString() + " ");
		// LOG(";");
		if (!validMoves.empty()) {
			// DLOGLN("FALSE");
			return false;
		}
	}
	// LOGLN();
	return true;
}

bool ClassicChessRules::isDraw(const ChessGameState& state) const {
	// 1. Stalemate
	CHESSCOLOR clr = state.getColorToMove();
	if (clr == CHESSCOLOR::UNKNOWN)
		return false;
	ChessPieceLocation locKing = state.findFirst(CHESSPIECE::KING, clr);
	if (!locKing.isOnBoard())
		return false;
	if (isMate(state))
		return false;
	// Serial.println("1. Stalemate");
	
	// 2. Dead position
	// TODO: implement

	// 3. Insufficient material
	static std::vector<std::pair<std::vector<ChessPiece>, std::vector<ChessPiece>>> cases = {
		{ { ChessPiece("K") }, { ChessPiece("k") } },
		{ { ChessPiece("K"), ChessPiece("B") }, { ChessPiece("k") } },
		{ { ChessPiece("K") }, { ChessPiece("k"), ChessPiece("b") } },
		{ { ChessPiece("K"), ChessPiece("N") }, { ChessPiece("k") } },
		{ { ChessPiece("K") }, { ChessPiece("k"), ChessPiece("n") } }
	};
	for (auto&& casePair : cases) {
		ChessGameState stateTemp(state);
		std::vector<ChessPiece> piecesSets[] = { casePair.first, casePair.second }; // white and black
		// first find and remove all the pieces from temp state
		for (uint8_t i = 0; i < 2; ++i) {
			std::vector<ChessPiece> pieces = piecesSets[i];
			for (const ChessPiece& piece : pieces)
				stateTemp.unset(stateTemp.findFirst(piece));
		}
		// if nothing's left in temp state, then it was the case -> draw
		bool somethingsLeft = false;
		for (const auto& piece : stateTemp.getPieces()) {
			if (piece.first.isOnBoard() && piece.second.isValid()) {
				somethingsLeft = true;
				break;
			}
		}
		if (!somethingsLeft)
			return true;
	}
	// 3.1 Extra case: King and bishop vs. king and bishop of the same color as the opponent's bishop
	// TODO: implement
	// Serial.println("3. Insufficient material");

	// 4. Threefold Repetition
	// Not tackable with this class. Should keep history (inherit and implement)

	// 5. 50-Move Rule
	if (state.getHalfMoves() >= 100)
		return true;
	// Serial.println("5. 50-Move Rule");
	
	return false;
}

std::vector<ChessMoveLocation> ClassicChessRules::getPossibleMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location, bool takesOnly) const {
	std::vector<ChessMoveLocation> moves;
	ChessPiece chessPiece = state.at(location);
	CHESSPIECE piece = chessPiece.getPiece();
	CHESSCOLOR clr = chessPiece.getColor();
	CHESSCOLOR clrOpposite = chessPiece.getColorOpposite();
	// Serial.print(F("Generating moves for piece "));
	// Serial.print(chessPiece.toString());
	// Serial.print(F(" at location "));
	// Serial.println(location.toString());
	if (piece == CHESSPIECE::UNKNOWN || clr == CHESSCOLOR::UNKNOWN) {
		// Serial.println(F("No piece at given location"));
		return moves;
	}
	uint8_t row = location._row;
	uint8_t col = location._col;

	static std::unordered_map<CHESSPIECE, std::vector<std::pair<int8_t, int8_t>>> dirsMap = {
		{ CHESSPIECE::KNIGHT, {{-2, -1}, {-2, 1}, {2, -1}, {2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}} },
		{ CHESSPIECE::BISHOP, {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}} },
		{ CHESSPIECE::ROOK, {{-1, 0}, {1, 0}, {0, -1}, {0, 1}} },
		{ CHESSPIECE::QUEEN, {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}, {-1, 0}, {1, 0}, {0, -1}, {0, 1}} },
		{ CHESSPIECE::KING, {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}, {-1, 0}, {1, 0}, {0, -1}, {0, 1}} }
	};
	switch (piece) {
		case CHESSPIECE::PAWN: {
			// Serial.println(F("> Pawn"));
			int8_t dir = clr == CHESSCOLOR::WHITE ? 1 : -1;
			// diagonal moves
			bool isOnEnPassantRow = location._row == ((dir + 1) / 2 + 3); // 4 for white, 3 for black
			for (int8_t dx = -1; dx < 2; dx += 2) {
				// ordinary diagonal
				ChessMoveLocation diagonal(static_cast<int8_t>(row) + dir, col + dx, true);
				if (diagonal.isOnBoard() && state.at(diagonal).getColor() == clrOpposite) {
					moves.push_back(diagonal);
					// Serial.print(F(">>> diagonal "));
					// Serial.println(dir > 0 ? F("right") : F("left"));
				}
				// en-passant
				if (!isOnEnPassantRow)
					continue;
				ChessMoveLocation diagEnPassant(static_cast<int8_t>(row), col + dx, true);
				if (diagEnPassant.isOnBoard()) {
					ChessPiece pieceOpp = state.at(diagEnPassant);
					if (pieceOpp.getColor() == clrOpposite && pieceOpp.getPiece() == CHESSPIECE::PAWN && pieceOpp.getHistory()) {
						moves.push_back(diagEnPassant);
						// Serial.print(F(">>> en-passant "));
						// Serial.println(dir > 0 ? F("right") : F("left"));
					}
				}
			}
			if (takesOnly)
				break;
			// single forward move
			ChessMoveLocation singleForward(static_cast<int8_t>(row) + dir, col);
			if (singleForward.isOnBoard() && state.at(singleForward).getColor() == CHESSCOLOR::UNKNOWN) {
				moves.push_back(singleForward);
				// Serial.println(F(">>> single forward"));
				// double forward move
				bool isOnInitialRow = location._row == ((7 + dir) % 7); // 1 for white, 6 for black
				ChessMoveLocation doubleForward(static_cast<int8_t>(row) + dir * 2, col);
				if (isOnInitialRow && doubleForward.isOnBoard() && state.at(doubleForward).getColor() == CHESSCOLOR::UNKNOWN) {
					moves.push_back(doubleForward);
					// Serial.println(F(">>> double forward"));
				}
			}
			// TODO: handle promotion
			break;
		}
		case CHESSPIECE::KNIGHT:
		case CHESSPIECE::BISHOP:
		case CHESSPIECE::ROOK:
		case CHESSPIECE::QUEEN:
		case CHESSPIECE::KING: {
			// TODO: handle castling
			// Serial.print(F("> "));
			// Serial.println(chessPiece.toString());
			auto dirsEntry = dirsMap.find(piece);
			if (dirsEntry == dirsMap.end()) // sanity check, normally doesn't reach here at all
				return std::vector<ChessMoveLocation>();
			std::vector<std::pair<int8_t, int8_t>>& dirs = dirsEntry->second;
			for (const auto& dir : dirs) {
				ChessMoveLocation loc(row + dir.first, col + dir.second);
				while (loc.isOnBoard()) {
					ChessPiece curPiece = state.at(loc);
					CHESSCOLOR curClr = curPiece.getColor();
					if (curClr == clr)
						break;
					if (curClr == clrOpposite)
						loc._take = true;
					if (!takesOnly || loc._take)
						moves.push_back(loc);
					if (curClr != CHESSCOLOR::UNKNOWN || piece == CHESSPIECE::KNIGHT || piece == CHESSPIECE::KING) // TODO: store flag for that in the map
						break;
					loc = ChessMoveLocation(loc._row + dir.first, loc._col + dir.second);
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

std::vector<ChessMoveLocation> ClassicChessRules::getValidMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location) const {
	// DLOGLN("getValidMovesForPiece: ");
	std::vector<ChessMoveLocation> possibleMoves = getPossibleMovesForPiece(state, location);
	if (possibleMoves.empty())
		return possibleMoves;
	
	ChessPiece piece = state.at(location);
	possibleMoves.erase(std::remove_if(possibleMoves.begin(), possibleMoves.end(), [&](const ChessMoveLocation& move) {
		ChessGameState stateTemp(state);
		// LOG("Pieces: ");
		// for (auto& p : stateTemp.getPieces())
		// 	LOG(p.first.toString() + String("(") + p.second.toString() + "),");
		// LOGLN();
		stateTemp.unset(location);
		stateTemp.set(move, piece);
		// DLOGLN(String("Moved ") + piece.toString() + " from " + location.toString() + " to " + move.toString());
		// LOG("Pieces: ");
		// for (auto& p : stateTemp.getPieces())
		// 	LOG(p.first.toString() + String("(") + p.second.toString() + "),");
		// LOGLN();
		bool check = isCheck(stateTemp, piece.getColor());
		return check;
	}), possibleMoves.end());

	return possibleMoves;
}