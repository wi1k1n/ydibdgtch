#ifndef RULESENGINE_H__
#define RULESENGINE_H__

#include "constants.h"

// #include "vector"
#include "unordered_map"

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
	uint8_t _color : 1;
	uint8_t _piece : 6;
#ifdef _DEBUG_
	static char _pieceSymbols[];
#endif

	ChessPiece() = default;
	ChessPiece(CHESSPIECE piece = CHESSPIECE::UNKNOWN, CHESSCOLOR color = CHESSCOLOR::UNKNOWN)
		: _piece(static_cast<uint8_t>(piece)), _color(static_cast<uint8_t>(color)) { }
	ChessPiece(const ChessPiece& piece) {
		_color = piece._color;
		_piece = piece._piece;
	}
	~ChessPiece() {}

	void setPiece(CHESSPIECE piece) { _piece = static_cast<uint8_t>(piece); }
	void setColor(CHESSCOLOR color) { _color = static_cast<uint8_t>(color); }

	bool operator==(const ChessPiece& other) const { return _color == other._color && _piece == other._piece; }

	String toString() const { return String(_color ? 'b' : 'w') + _pieceSymbols[_piece]; }
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
	~ChessPieceLocation() {}

	void setLocation(uint8_t row, uint8_t col) { _row = row; _col = col; }

	bool operator==(const ChessPieceLocation& other) const { return _row == other._row && _col == other._col; }

	String toString() const { return String(static_cast<char>('a' + _row)) + _col; }
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

	void _fillRow(uint8_t row, CHESSPIECE piece, CHESSCOLOR color) {
		for (uint8_t i = 0; i < 8; ++i)
			_pieces.emplace(std::make_pair(ChessPieceLocation{ row, i }, ChessPiece{ piece, color }));
	}
	void _fillRow(uint8_t row, const std::initializer_list<CHESSPIECE>& pieces, CHESSCOLOR color) {
		auto it = pieces.begin();
		for (uint8_t i = 0; i < 8 && it != pieces.end(); ++i, ++it)
			_pieces.emplace(std::make_pair(ChessPieceLocation{ row, i }, ChessPiece{ *it, color }));
	}
	void _fillCol(uint8_t col, CHESSPIECE piece, CHESSCOLOR color) {
		for (uint8_t i = 0; i < 8; ++i)
			_pieces.emplace(std::make_pair(ChessPieceLocation{ i, col }, ChessPiece{ piece, color }));
	}
	void _fillCol(uint8_t col, const std::initializer_list<CHESSPIECE>& pieces, CHESSCOLOR color) {
		auto it = pieces.begin();
		for (uint8_t i = 0; i < 8 && it != pieces.end(); ++i, ++it)
			_pieces.emplace(std::make_pair(ChessPieceLocation{ i, col }, ChessPiece{ *it, color }));
	}
public:
	ChessGameState(const CHESSINITIALSTATE& initState = CHESSINITIALSTATE::CLASSIC, const CHESSCOLOR& colorToMove = CHESSCOLOR::WHITE) {
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
	
	String toString() const {
		String res;
		for (uint8_t row = 0; row < 8; ++row) {
			for (uint8_t col = 0; col < 8; ++col) {
				if (auto entry = _pieces.find({ row, col }); entry != _pieces.end())
					res += entry->second.toString();
				else
					res += "  ";
				res += "|";
			}
			res += '\n';
		}
		return res;
	}
	bool operator==(const ChessGameState& other) {
		return _pieces == other._pieces;
	}
};

// Constructs game states dependencies and resolves state changes
class ChessGameStatesResolver {

};

class ChessRulesEngine {
	virtual ChessGameState getStartingState() const { return ChessGameState(); }
	virtual bool isStateValid(const ChessGameState& state) const { return true; }
	virtual bool isStateChangeValid(const ChessGameState& state1, const ChessGameState& state2) const { return true; }
};

class ClassicChessRules : public ChessRulesEngine {
	ChessGameState getStartingState() const override { return ChessGameState(CHESSINITIALSTATE::CLASSIC); }
};

#endif // RULESENGINE_H__