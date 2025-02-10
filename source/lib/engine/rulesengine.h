#ifndef RULESENGINE_H__
#define RULESENGINE_H__

#include "chs_types.h"
#include "chs_string.h"

enum class CHESSPIECE : uint8_t {
	UNKNOWN = 255,
	PAWN = 0,
	KNIGHT = 1,
	BISHOP = 2,
	ROOK = 3,
	QUEEN = 4,
	KING = 5,
};

enum class CHESSCOLOR : uint8_t {
	UNKNOWN = 255,
	WHITE = 0,
	BLACK = 1,
	RED = 2,
	BLUE = 3,
	GREEN = 4,
	YELLOW = 5,
};

// TODO: redundant as can be expressed by 2 bits (king castle, queen castle) and ChessPieceLocation (en-passant)
enum class CHESSHISTORY : uint8_t {
	UNKNOWN = 255,
	NONE = 0,
	MOVED = 1, // the piece has already moved once before (e.g. for castling)
	ENPASSANT_VULNERABLE = 2, // the piece is vulnerable to en-passant
};

enum class CHESSINITSTATE : uint8_t {
	UNKNOWN = 255,
	EMPTY = 0,
	CLASSIC = 1,
};

enum class CHESSMOVEINFO : uint8_t {
	UNKNOWN = 255,
	NONE = 0, // ordinary move
	TAKING = 1, // the move is taking another piece
	CASTLING = 2, // the move is castling
	PROMOTION = 3, // the move is pawn promotion
};

template<> struct std::hash<CHESSPIECE> {
	size_t operator()(const CHESSPIECE& other) const {
		return std::hash<uint8_t>()(static_cast<uint8_t>(other));
	}
};

const int8_t CHESSLOCATION_INVALID = -1;

/// @brief Solely chess piece representation (without any location or game state data)
class ChessPiece {
public:
	ChessPiece(
		CHESSPIECE piece = CHESSPIECE::UNKNOWN,
		CHESSCOLOR color = CHESSCOLOR::UNKNOWN,
		CHESSHISTORY history = CHESSHISTORY::NONE
	)
		: m_piece(piece), m_color(color), m_history(history) { }
	ChessPiece(const ChessPiece& piece)
		: m_piece(piece.m_piece), m_color(piece.m_color), m_history(piece.m_history) { }
	~ChessPiece() {}

	/// @brief Initialize piece using FEN notation (white/black mode only)
	ChessPiece(char c, CHESSHISTORY history = CHESSHISTORY::NONE);
	ChessPiece(const String& s, CHESSHISTORY history = CHESSHISTORY::NONE);

	inline CHESSPIECE GetPiece() const { return m_piece; }
	inline CHESSCOLOR GetColor() const { return m_color; }
	inline CHESSHISTORY GetHistory() const { return m_history; }
	void SetPiece(CHESSPIECE piece) { m_piece = piece; }
	void SetColor(CHESSCOLOR color) { m_color = color; }
	void SetHistory(CHESSHISTORY history) { m_history = history; }

	/// @brief Returns the color opposite to the current one (white/black mode only)
	/// @return Black if current is white, white if current is black, otherwise UNKNOWN
	inline CHESSCOLOR GetColorOpposite() const {
		const CHESSCOLOR clr = GetColor();
		return (clr == CHESSCOLOR::BLACK) ? CHESSCOLOR::WHITE : (clr == CHESSCOLOR::WHITE ? CHESSCOLOR::BLACK : CHESSCOLOR::UNKNOWN);
	}

	inline bool IsValid() const {
		return GetPiece() != CHESSPIECE::UNKNOWN
			&& GetColor() != CHESSCOLOR::UNKNOWN
			&& GetHistory() != CHESSHISTORY::UNKNOWN;
	}

	String ToString(bool symbolic = true) const;

	bool operator==(const ChessPiece& other) const {
		return m_color == other.m_color && m_piece == other.m_piece && m_history == other.m_history;
	}

private:
	void init(char c, CHESSHISTORY history);

private:
	CHESSPIECE m_piece = CHESSPIECE::UNKNOWN;
	CHESSCOLOR m_color = CHESSCOLOR::UNKNOWN;
	CHESSHISTORY m_history = CHESSHISTORY::NONE;
};

template<> struct std::hash<ChessPiece> {
	size_t operator()(const ChessPiece& other) const {
		return std::hash<CHESSPIECE>()(other.GetPiece()) ^ std::hash<CHESSCOLOR>()(other.GetColor()) ^ std::hash<CHESSHISTORY>()(other.GetHistory());
	}
};

/// @brief Solely chess piece location representation (without any piece or game state data)
class ChessPieceLocation {
public:
	ChessPieceLocation() = default;
	ChessPieceLocation(const ChessPieceLocation& piece)
		: m_row(piece.m_row), m_col(piece.m_col) {}
	~ChessPieceLocation() {}

	ChessPieceLocation(int8_t row, int8_t col)
		: m_row(row), m_col(col) {}
	ChessPieceLocation(int8_t idx)
		: m_row(idx / 8), m_col(idx % 8) {}
	ChessPieceLocation(std::pair<int8_t, int8_t> pair)
		: m_row(pair.first), m_col(pair.second) {}
	ChessPieceLocation(const String& s);

	inline int8_t GetRow() const { return m_row; }
	inline int8_t GetCol() const { return m_col; }

	void SetLocation(int8_t row, int8_t col) { m_row = row; m_col = col; }
	inline bool IsOnBoard() const { return m_col >= 0 && m_col < 8 && m_row >= 0 && m_row < 8; }

	bool operator==(const ChessPieceLocation& other) const { return m_row == other.m_row && m_col == other.m_col; }

	String ToString(bool symbolic = true) const {
		if (symbolic)
			return String(static_cast<char>('a' + m_col)) + String(m_row + 1);
		return String(m_row) + ":" + String(m_col);
	}

private:
	int8_t m_row = CHESSLOCATION_INVALID;
	int8_t m_col = CHESSLOCATION_INVALID;
};

/// @brief Same as ChessPieceLocation, but keeps additional move information
class ChessMoveLocation : public ChessPieceLocation {
public:
	ChessMoveLocation() = default;
	ChessMoveLocation(const ChessMoveLocation& piece)
		: ChessPieceLocation(piece), m_moveInfo(piece.m_moveInfo), m_promotionTarget(piece.m_promotionTarget) {}
	~ChessMoveLocation() {}

	ChessMoveLocation(int8_t row, int8_t col, CHESSMOVEINFO moveInfo = CHESSMOVEINFO::NONE, CHESSPIECE promotionTarget = CHESSPIECE::UNKNOWN)
		: ChessPieceLocation(row, col), m_moveInfo(moveInfo), m_promotionTarget(promotionTarget) {}
	ChessMoveLocation(const String& s)
		: ChessPieceLocation(s) {}

	CHESSMOVEINFO GetMoveInfo() const { return m_moveInfo; }
	CHESSPIECE GetPromotionTarget() const { return m_promotionTarget; }

	void SetMoveInfo(CHESSMOVEINFO moveInfo) { m_moveInfo = moveInfo; }
	void SetPromotionTarget(CHESSPIECE promotionTarget) { m_promotionTarget = promotionTarget; }

	bool operator==(const ChessMoveLocation& other) const {
		return ChessPieceLocation::operator==(other) && m_moveInfo == other.m_moveInfo && m_promotionTarget == other.m_promotionTarget;
	}

private:
	CHESSMOVEINFO m_moveInfo = CHESSMOVEINFO::NONE;
	CHESSPIECE m_promotionTarget = CHESSPIECE::UNKNOWN; // only used if m_moveInfo == CHESSMOVEINFO::PROMOTION
};

/// @brief Move representation (doesn't make any sense without a game state)
struct ChessStateMove {
	ChessPieceLocation from;
	ChessMoveLocation to;

	ChessStateMove() = default;
	ChessStateMove(const ChessPieceLocation& from, const ChessMoveLocation& to)
		: from(from), to(to) {}
};

template<> struct std::hash<ChessPieceLocation> {
	size_t operator()(const ChessPieceLocation& other) const {
		return std::hash<int8_t>()(other.GetRow() << 4) ^ std::hash<int8_t>()(other.GetCol());
	}
};

/// @brief The chess game state representation: keeps track of all the pieces on the board and other game state data
class ChessGameState {
public:
	/// @brief Create a game state that is intentionally in the undefined/invalid state
	static ChessGameState GetUndefinedState() { return ChessGameState(CHESSINITSTATE::UNKNOWN, CHESSCOLOR::UNKNOWN); }

public:
	ChessGameState(CHESSINITSTATE initState = CHESSINITSTATE::CLASSIC, CHESSCOLOR colorToMove = CHESSCOLOR::WHITE);
	ChessGameState(const ChessGameState& other)
		: m_pieces(other.m_pieces), m_colorToMove(other.m_colorToMove), m_fullMoves(other.m_fullMoves), m_halfMoves(other.m_halfMoves) {}
	ChessGameState(const ChessGameState&& other) noexcept
		: m_pieces(other.m_pieces), m_colorToMove(other.m_colorToMove), m_fullMoves(other.m_fullMoves), m_halfMoves(other.m_halfMoves) {}
	~ChessGameState() {}

	ChessGameState& operator=(const ChessGameState& other) {
		m_pieces = other.m_pieces;
		m_colorToMove = other.m_colorToMove;
		m_fullMoves = other.m_fullMoves;
		m_halfMoves = other.m_halfMoves;
		return *this;
	}
	ChessGameState& operator=(const ChessGameState&& other) noexcept {
		m_pieces = other.m_pieces;
		m_colorToMove = other.m_colorToMove;
		m_fullMoves = other.m_fullMoves;
		m_halfMoves = other.m_halfMoves;
		return *this;
	}

	ChessGameState(const String& fenString, bool allowPartial = true);

	inline const Hashmap<ChessPieceLocation, ChessPiece>& GetPieces() const { return m_pieces; }

	inline CHESSCOLOR GetColorToMove() const { return m_colorToMove; }
	inline uint16_t GetFullMoves() const { return m_fullMoves; }
	inline uint8_t GetHalfMoves() const { return m_halfMoves; }

	void SetColorToMove(CHESSCOLOR color) { m_colorToMove = color; }
	void SetFullMoves(uint16_t fullMoves) { m_fullMoves = fullMoves; }
	void SetHalfMoves(uint8_t halfMoves) { m_halfMoves = halfMoves; }

	bool IsLocationOccupied(const ChessPieceLocation& location) const;

	ChessPieceLocation FindFirst(CHESSPIECE piece, CHESSCOLOR color = CHESSCOLOR::UNKNOWN) const;
	ChessPieceLocation FindFirst(const ChessPiece& piece) const;

	ChessPiece At(const ChessPieceLocation& location) const;
	ChessPiece At(uint8_t row, uint8_t col) const;
	ChessPiece At(uint8_t idx) const;
	ChessPiece At(const String& s) const; // only lower case!
	
	void Set(const ChessPieceLocation& location, const ChessPiece& piece);
	void Set(uint8_t row, uint8_t col, const ChessPiece& piece);
	void Set(const String& location, const ChessPiece& piece);
	void Set(const String& location, const String& piece);
	void Unset(const ChessPieceLocation& location);
	void Unset(uint8_t row, uint8_t col);
	void Unset(const String& location);

	bool MakeMove(const ChessStateMove& move);
	
	/// @brief Switches the turn to the other player (white/black mode only)
	void NextTurn();

	String ToString(bool legend = true, bool transpose = true, bool zeroBased = false) const;
	String ToFEN() const;

	bool operator==(const ChessGameState& other) const;

private:
	void fillRow(uint8_t row, CHESSPIECE piece, CHESSCOLOR color);
	void fillRow(uint8_t row, const std::initializer_list<CHESSPIECE>& pieces, CHESSCOLOR color);
	void fillCol(uint8_t col, CHESSPIECE piece, CHESSCOLOR color);
	void fillCol(uint8_t col, const std::initializer_list<CHESSPIECE>& pieces, CHESSCOLOR color);

	/// @param allowPartial Still initialize even if FEN is incomplete
	bool initFromFEN(const String& fenString, bool allowPartial = true);

private:
	Hashmap<ChessPieceLocation, ChessPiece> m_pieces;
	CHESSCOLOR m_colorToMove = CHESSCOLOR::WHITE;
	uint16_t m_fullMoves = 1;
	uint8_t m_halfMoves = 0;
};

/// @brief Virtual class for chess rules engine
class ChessRulesEngine {
public:
	ChessRulesEngine() = default;
	ChessRulesEngine(const ChessRulesEngine&) = delete;
	void operator=(const ChessRulesEngine&) = delete;

	virtual ChessGameState GetStartingState() const = 0;

	/// @brief All possible movements for a piece on the board. Takes into account possibility to take and inability to move to an occupied square. Doesn't check for the move's legality.
	/// @param takesOnly Filters out all the non-taking moves
	virtual Array<ChessMoveLocation> GetPossibleMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location, bool takesOnly = false) const = 0;

	/// @brief Same as GetPossibleMovesForPiece(), but also cheks for moves legality (e.g. if moves are not possible due to opening king for a check)
	virtual Array<ChessMoveLocation> GetValidMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location) const = 0;


	virtual String ToString() const { return "ChessRulesEngine"; }
};

template<> struct std::hash<ChessGameState> {
	size_t operator()(const ChessGameState& other) const {
		size_t hash = 0;
		for (const auto& pair : other.GetPieces())
			hash ^= std::hash<ChessPieceLocation>()(pair.first) ^ std::hash<ChessPiece>()(pair.second);
		return hash ^ std::hash<CHESSCOLOR>()(other.GetColorToMove()) ^ std::hash<uint16_t>()(other.GetFullMoves()) ^ std::hash<uint8_t>()(other.GetHalfMoves());
	}
};

/// @brief Classic chess rules engine
class ClassicChessRules : public ChessRulesEngine {
public:
	ChessGameState GetStartingState() const override { return ChessGameState(CHESSINITSTATE::CLASSIC, CHESSCOLOR::WHITE); }
	Array<ChessMoveLocation> GetPossibleMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location, bool takesOnly = false) const override;
	Array<ChessMoveLocation> GetValidMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location) const override;

	bool IsCheck(const ChessGameState& state, CHESSCOLOR color = CHESSCOLOR::UNKNOWN) const;
	bool IsMate(const ChessGameState& state, CHESSCOLOR color = CHESSCOLOR::UNKNOWN) const;
	bool IsDraw(const ChessGameState& state) const;
private:
};

//class ChessRulesEngine {
//public:
//	ChessRulesEngine() = default;
//	ChessRulesEngine(const ChessRulesEngine&) = delete;
//	void operator=(const ChessRulesEngine&) = delete; 
//	
//	virtual ChessGameState getStartingState() const = 0;
//
//	// All possible movements for a piece on the board. Takes into account possibility to take and inability to move to an occupied square. Doesn't check for the move's legality.
//	// takesOnly filters out all the non-taking moves
//	virtual std::vector<ChessMoveLocation> getPossibleMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location, bool takesOnly = false) const = 0;
//	// Same as getPossibleMovesForPiece(), but also cheks for moves legality (e.g. if moves are not possible due to opening king for a check)
//	virtual std::vector<ChessMoveLocation> getValidMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location) const = 0;
//
//	// virtual bool isStateValid(const ChessGameState& state) const = 0;
//	// virtual bool isStateChangeValid(const ChessGameState& state1, const ChessGameState& state2) const = 0;
//	// virtual bool isMoveAllowed(const ChessMove& move) = 0;
//
//	virtual String toString() const { return "ChessRulesEngine"; }
//};
//

#endif // RULESENGINE_H__