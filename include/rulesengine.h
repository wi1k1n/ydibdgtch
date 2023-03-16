#ifndef RULESENGINE_H__
#define RULESENGINE_H__

#include "constants.h"
#include "utilities.h"

#include <vector>
#include <unordered_map>

enum class CHESSPIECE {
	UNKNOWN = -1,
	PAWN = 0,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING
};
enum class CHESSCOLOR {
	UNKNOWN = -1,
	WHITE = 0,
	BLACK
};
enum class CHESSINITIALSTATE {
	UNKNOWN = -1,
	CLASSIC = 0,
	EMPTY
};

struct ChessPiece {
	// getters should be adjusted if bitness is changed
	uint8_t _piece : 4;
	uint8_t _color : 2;
	uint8_t _history : 1; // extra info flag - king: was moved before; pawn: susceptible to en-passant
#ifdef _DEBUG_
	static char _pieceSymbols[];
#endif

	ChessPiece(CHESSPIECE piece = CHESSPIECE::UNKNOWN, CHESSCOLOR color = CHESSCOLOR::UNKNOWN, bool history = false)
		: _piece(static_cast<uint8_t>(piece)), _color(static_cast<uint8_t>(color)), _history(history) { }
	ChessPiece(const ChessPiece& piece) 
		: _piece(piece._piece), _color(piece._color), _history(piece._history) { }
	ChessPiece(const String& s);
	~ChessPiece() {}

	void setPiece(CHESSPIECE piece) { _piece = static_cast<uint8_t>(piece); }
	void setColor(CHESSCOLOR color) { _color = static_cast<uint8_t>(color); }
	void setHistory(bool value) { _history = value; }
	CHESSPIECE getPiece() const { return static_cast<CHESSPIECE>(_piece == 0b00001111 ? -1 : _piece); }
	CHESSCOLOR getColor() const { return static_cast<CHESSCOLOR>(_color == 0b00000011 ? -1 : _color); }
	bool getHistory() const { return static_cast<bool>(_history); }

	bool operator==(const ChessPiece& other) const { return _color == other._color && _piece == other._piece; }

	String toString(bool symbolic = true) const;
};
struct ChessPieceLocation {
	uint8_t _row : 3;
	uint8_t _col : 3;

	ChessPieceLocation() = default;
	ChessPieceLocation(uint8_t row, uint8_t col)
		: _row(row), _col(col) { }
	ChessPieceLocation(const ChessPieceLocation& piece) {
		setLocation(piece._row, piece._col);
	}
	ChessPieceLocation(const String& s);
	~ChessPieceLocation() {}

	void setLocation(uint8_t row, uint8_t col) { _row = row; _col = col; }
	bool isOnBoard() const { return _col >= 0 && _col < 8 && _row >= 0 && _row < 8; }

	bool operator==(const ChessPieceLocation& other) const { return _row == other._row && _col == other._col; }

	String toString(bool symbolic = true) const {
		if (symbolic)
			return String(static_cast<char>('a' + _col)) + String(_row + 1);
		return String(_row) + ":" + String(_col);
	}
};

namespace std {
template<>
struct hash<ChessPieceLocation> {
	size_t operator()(const ChessPieceLocation& other) const {
		return hash<uint8_t>()(other._row) ^ hash<uint8_t>()(other._col);
	}
};
} // namespace std

class ChessGameState {
	// std::vector<ChessPiece> _pieces;
	std::unordered_map<ChessPieceLocation, ChessPiece> _pieces;
	CHESSCOLOR _colorToMove = CHESSCOLOR::WHITE;

	void _fillRow(uint8_t row, CHESSPIECE piece, CHESSCOLOR color);
	void _fillRow(uint8_t row, const std::initializer_list<CHESSPIECE>& pieces, CHESSCOLOR color);
	void _fillCol(uint8_t col, CHESSPIECE piece, CHESSCOLOR color);
	void _fillCol(uint8_t col, const std::initializer_list<CHESSPIECE>& pieces, CHESSCOLOR color);
public:
	ChessGameState(const CHESSINITIALSTATE& initState = CHESSINITIALSTATE::CLASSIC, const CHESSCOLOR& colorToMove = CHESSCOLOR::WHITE);
	ChessGameState(const ChessGameState& other);

	ChessPiece at(const ChessPieceLocation& location) const;
	ChessPiece at(uint8_t row, uint8_t col) const;
	ChessPiece at(const String& s) const; // only lower case!
	void set(const ChessPieceLocation& location, const ChessPiece& piece);
	void set(uint8_t row, uint8_t col, const ChessPiece& piece);
	void set(const String& location, const ChessPiece& piece);
	void set(const String& location, const String& piece);
	
	bool isLocationOccupied(const ChessPieceLocation& location) const;
	
	String toString(bool legend = true, bool transpose = true, bool zeroBased = false) const;
	bool operator==(const ChessGameState& other) const { return _pieces == other._pieces; }
};

typedef std::pair<ChessPieceLocation, ChessPieceLocation> ChessMove;
typedef std::pair<ChessGameState, ChessMove> ChessTurn;

// Constructs game states dependencies and resolves state changes
class ChessGameStatesResolver {

};

class ChessRulesEngine {
public:
	ChessRulesEngine() = default;
	ChessRulesEngine(const ChessRulesEngine&) = delete;
	void operator=(const ChessRulesEngine&) = delete; 
	
	virtual ChessGameState getStartingState() const = 0;
	virtual std::vector<ChessPieceLocation> getValidMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location) const = 0;

	// virtual bool isStateValid(const ChessGameState& state) const = 0;
	// virtual bool isStateChangeValid(const ChessGameState& state1, const ChessGameState& state2) const = 0;
	// virtual bool isMoveAllowed(const ChessMove& move) = 0;

	virtual String toString() const { return "ChessRulesEngine"; }
};

class ClassicChessRules : public ChessRulesEngine {
public:
	// static ClassicChessRules& Get() { static ClassicChessRules instance; return instance; };

	ChessGameState getStartingState() const override { return ChessGameState(); };
	std::vector<ChessPieceLocation> getValidMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location) const override;

	String toString() const override { return "ClassicChessRules"; }
};

#endif // RULESENGINE_H__