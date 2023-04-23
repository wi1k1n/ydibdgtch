#ifndef RULESENGINE_H__
#define RULESENGINE_H__

#include "sdk.h"
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
	static char _pieceSymbols[];

	// getters should be adjusted if bitness is changed
	uint8_t _piece : 4;
	uint8_t _color : 2;
	uint8_t _history : 1; // extra info flag - king/rook: was moved before; pawn: susceptible to en-passant

	ChessPiece(CHESSPIECE piece = CHESSPIECE::UNKNOWN, CHESSCOLOR color = CHESSCOLOR::UNKNOWN, bool history = false)
		: _piece(static_cast<uint8_t>(piece)), _color(static_cast<uint8_t>(color)), _history(history) { }
	ChessPiece(const ChessPiece& piece) 
		: _piece(piece._piece), _color(piece._color), _history(piece._history) { }
	ChessPiece(const String& s);
	ChessPiece(char c);
	~ChessPiece() {}

	void setPiece(CHESSPIECE piece) { _piece = static_cast<uint8_t>(piece); }
	void setColor(CHESSCOLOR color) { _color = static_cast<uint8_t>(color); }
	void setHistory(bool value) { _history = value; }
	inline CHESSPIECE getPiece() const { return static_cast<CHESSPIECE>(_piece == 0b00001111 ? -1 : _piece); }
	inline CHESSCOLOR getColor() const { return static_cast<CHESSCOLOR>(_color == 0b00000011 ? -1 : _color); }
	inline CHESSCOLOR getColorOpposite() const { CHESSCOLOR clr = getColor(); return (clr == CHESSCOLOR::BLACK) ? CHESSCOLOR::WHITE : (clr == CHESSCOLOR::WHITE ? CHESSCOLOR::BLACK : clr); }
	bool getHistory() const { return static_cast<bool>(_history); }

	inline bool isValid() const { return getPiece() != CHESSPIECE::UNKNOWN && getColor() != CHESSCOLOR::UNKNOWN; }

	bool operator==(const ChessPiece& other) const { return _color == other._color && _piece == other._piece; }

	String toString(bool symbolic = true) const;
private:
	void _initFromChar(char c);
};
struct ChessPieceLocation {
	int8_t _row : 4;
	int8_t _col : 4;

	ChessPieceLocation() = default;
	ChessPieceLocation(int8_t idx)
		: _row(idx / 8), _col(idx % 8) { }
	ChessPieceLocation(int8_t row, int8_t col)
		: _row(row), _col(col) { }
	ChessPieceLocation(std::pair<int8_t, int8_t> pair)
		: _row(pair.first), _col(pair.second) { }
	ChessPieceLocation(const ChessPieceLocation& piece) {
		setLocation(piece._row, piece._col);
	}
	ChessPieceLocation(const String& s);
	~ChessPieceLocation() {}

	void setLocation(int8_t row, int8_t col) { _row = row; _col = col; }
	inline bool isOnBoard() const { return isOnBoard(_row, _col); }

	static bool isOnBoard(int8_t row, int8_t col) { return col >= 0 && col < 8 && row >= 0 && row < 8; }

	bool operator==(const ChessPieceLocation& other) const { return _row == other._row && _col == other._col; }

	String toString(bool symbolic = true) const {
		if (symbolic)
			return String(static_cast<char>('a' + _col)) + String(_row + 1);
		return String(_row) + ":" + String(_col);
	}
};

struct ChessMoveLocation : public ChessPieceLocation {
	bool _take = 0;

	ChessMoveLocation(const ChessMoveLocation& piece) : ChessPieceLocation(piece), _take(piece._take) { }
	ChessMoveLocation(int8_t row, int8_t col, bool take = false) : ChessPieceLocation(row, col), _take(take) { }
	ChessMoveLocation(std::pair<int8_t, int8_t> pair, bool take = false) : ChessPieceLocation(pair), _take(take) { }
	bool isTaking() const { return _take; }
};

namespace std {
template<>
struct hash<ChessPieceLocation> {
	size_t operator()(const ChessPieceLocation& other) const {
		return hash<int8_t>()(other._row) ^ hash<int8_t>()(other._col);
	}
};
template<>
struct hash<CHESSPIECE> {
	size_t operator()(const CHESSPIECE& other) const {
		return hash<int8_t>()(static_cast<int8_t>(other));
	}
};
} // namespace std

class ChessGameState {
	// std::vector<ChessPiece> _pieces;
	std::unordered_map<ChessPieceLocation, ChessPiece> _pieces;
	CHESSCOLOR _colorToMove = CHESSCOLOR::WHITE;
	uint16_t _fullMoves = 1;
	uint8_t _halfMoves = 0;

	void _fillRow(uint8_t row, CHESSPIECE piece, CHESSCOLOR color);
	void _fillRow(uint8_t row, const std::initializer_list<CHESSPIECE>& pieces, CHESSCOLOR color);
	void _fillCol(uint8_t col, CHESSPIECE piece, CHESSCOLOR color);
	void _fillCol(uint8_t col, const std::initializer_list<CHESSPIECE>& pieces, CHESSCOLOR color);

	bool _initFromFEN(const String& fenString);
public:
	ChessGameState(const CHESSINITIALSTATE& initState = CHESSINITIALSTATE::CLASSIC, const CHESSCOLOR& colorToMove = CHESSCOLOR::WHITE);
	ChessGameState(const ChessGameState& other);
	ChessGameState(const String& fenString);

	ChessPiece at(const ChessPieceLocation& location) const;
	ChessPiece at(uint8_t row, uint8_t col) const;
	ChessPiece at(const String& s) const; // only lower case!
	void set(const ChessPieceLocation& location, const ChessPiece& piece);
	void set(uint8_t row, uint8_t col, const ChessPiece& piece);
	void set(const String& location, const ChessPiece& piece);
	void set(const String& location, const String& piece);
	void unset(const ChessPieceLocation& location);
	void unset(uint8_t row, uint8_t col);
	void unset(const String& location);

	ChessPieceLocation findFirst(CHESSPIECE piece, CHESSCOLOR color = CHESSCOLOR::UNKNOWN) const;
	ChessPieceLocation findFirst(const ChessPiece& piece) const;

	// TODO: implement iterators
	const std::unordered_map<ChessPieceLocation, ChessPiece>& getPieces() const { return _pieces; }
	CHESSCOLOR getColorToMove() const { return _colorToMove; }
	uint16_t getFullMoves() const { return _fullMoves; }
	uint8_t getHalfMoves() const { return _halfMoves; }
	
	bool isLocationOccupied(const ChessPieceLocation& location) const;
	
	String toString(bool legend = true, bool transpose = true, bool zeroBased = false) const;
	String toFEN() const;
	bool operator==(const ChessGameState& other) const { return _pieces == other._pieces; }
};

typedef std::pair<ChessPieceLocation, ChessPieceLocation> ChessMove;
typedef std::pair<ChessGameState, ChessMove> ChessTurn;

class ChessRulesEngine {
public:
	ChessRulesEngine() = default;
	ChessRulesEngine(const ChessRulesEngine&) = delete;
	void operator=(const ChessRulesEngine&) = delete; 
	
	virtual ChessGameState getStartingState() const = 0;

	// All possible movements for a piece on the board. Takes into account possibility to take and inability to move to an occupied square. Doesn't check for the move's legality.
	// takesOnly filters out all the non-taking moves
	virtual std::vector<ChessMoveLocation> getPossibleMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location, bool takesOnly = false) const = 0;
	// Same as getPossibleMovesForPiece(), but also cheks for moves legality (e.g. if moves are not possible due to opening king for a check)
	virtual std::vector<ChessMoveLocation> getValidMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location) const = 0;

	// virtual bool isStateValid(const ChessGameState& state) const = 0;
	// virtual bool isStateChangeValid(const ChessGameState& state1, const ChessGameState& state2) const = 0;
	// virtual bool isMoveAllowed(const ChessMove& move) = 0;

	virtual String toString() const { return "ChessRulesEngine"; }
};

class ClassicChessRules : public ChessRulesEngine {
public:
	// static ClassicChessRules& Get() { static ClassicChessRules instance; return instance; };

	ChessGameState getStartingState() const override { return ChessGameState(); };
	std::vector<ChessMoveLocation> getPossibleMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location, bool takesOnly = false) const override;
	std::vector<ChessMoveLocation> getValidMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location) const override;

	// returns if "color" colored king is in check
	bool isCheck(const ChessGameState& state, CHESSCOLOR color = CHESSCOLOR::UNKNOWN) const;
	bool isMate(const ChessGameState& state, CHESSCOLOR color = CHESSCOLOR::UNKNOWN) const;
	bool isDraw(const ChessGameState& state) const;

	String toString() const override { return "ClassicChessRules"; }
};

#endif // RULESENGINE_H__