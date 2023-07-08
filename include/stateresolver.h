#ifndef STATERESOLVER_H__
#define STATERESOLVER_H__

#include "rulesengine.h"
#include "senseboard.h"

#include <unordered_set>
#include <list>

// ------------------------------------------------------------------------------------
// ------ Data structure ------
// ------------------------------------------------------------------------------------

class GSBranch;

/// @brief GameStateTree. Main game states container interface
class GSTree {
	ChessGameState _initState;
	std::vector<GSBranch> _branches;
public:
	bool init(const ChessGameState& initState);

	const ChessGameState& getInitialState() const;
	uint8_t getBranchCount() const;
	const GSBranch* getBranch(uint8_t idx) const;
	GSBranch* getBranch(uint8_t idx);
};

enum class GSBoardMoveType {
	NONE = 0,
	DISAPPEARED,
	BACKONBOARD
};

struct GSBoardMove {
	GSBoardMoveType type = GSBoardMoveType::NONE;
	ChessPieceLocation pos;
	ChessPiece piece;

	GSBoardMove() = default;
	GSBoardMove(GSBoardMoveType type, ChessPieceLocation pos, ChessPiece piece)
		: type(type), pos(pos), piece(piece) {}
};

/// @brief GameStateBranch
class GSBranch {
	GSTree* _parentTree = nullptr;
	std::vector<GSBoardMove> _moves;
public:
	GSBranch(GSTree* parent);

	ChessGameState evaluateGameState() const;

	bool disappeared(const ChessPieceLocation& pos);
	bool backonboard(const ChessPieceLocation& pos);

	std::vector<GSBoardMove>& getMoves() { return _moves; }

	String toString() const;
};

// ------------------------------------------------------------------------------------
// ------ Interfaces ------
// ------------------------------------------------------------------------------------

/// @brief Detect sense board state changes and debounce 'em if exist
class SenseBoardStateDebouncer
{
	SenseBoardState _prev; // only used for debouncing
	uint32_t _time; ///< Timestamp when _prev was obtained
	std::vector<ChessPieceLocation> _lastDebouncedChanges;
public:
	SenseBoardStateDebouncer() = default;
	
	bool init(const SenseBoardState& initState) {
		_prev = initState;
		return true;
	}

	bool tick(const SenseBoardState& newState) {
		uint32_t curTime = millis();
		if (curTime - _time < SENSEBOARD_DEBOUNCE_TIME_MS) // debounce time has not still passed
			return false;
		_time = curTime;
		if (newState == _prev) // state hasn't changed
			return false;
		_lastDebouncedChanges = newState - _prev;
		_prev = newState;
		return true;
	}

	const std::vector<ChessPieceLocation>& getChanges() const { return _lastDebouncedChanges; }
};

/// @brief GameStateResolverInfo. A helper class that aggregates game states resolution information
struct GSResolverInfo {
	bool isInvalid = false;
	bool isIntermediate = false;
	bool isFinished = true; // state can be intermediate and finished at the same time

	GSResolverInfo() = default;
	GSResolverInfo(bool invalid, bool intermediate, bool finished)
	 : isInvalid(invalid), isIntermediate(intermediate), isFinished(finished)
	{}

	String toString() const {
		String res("{"_f);
		if (!isInvalid)
			res += "!"_f;
		res += "invalid "_f;
		if (!isIntermediate)
			res += "!"_f;
		res += "intermediate "_f;
		if (!isFinished)
			res += "!"_f;
		res += "finished}"_f;
		return res;
	}
};

class GSResolver {
	const ChessRulesEngine* _rules = nullptr;
	GSTree _tree;
	GSResolverInfo _info;
public:
	GSResolver() = default;

	// TODO: Need a function to manually set up current game state!
	bool init(const ChessRulesEngine& rules, const ChessGameState& initState);
	const GSResolverInfo& update(const std::vector<ChessPieceLocation>& changes);
	ChessGameState getGameState(uint8_t idx = 0) const;
private:
	const GSResolverInfo& invalidateInfo() { _info = {true, false, false}; return _info; }
};

#endif // STATERESOLVER_H__