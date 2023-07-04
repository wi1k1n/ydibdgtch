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
};

/// @brief GameStateBranch
class GSBranch {
	GSTree* _parentTree = nullptr;
	std::vector<ChessPieceLocation> _moves;
public:
	GSBranch(GSTree* parent);
	ChessGameState evaluateGameState() const;
	String toString() const;
};

// ------------------------------------------------------------------------------------
// ------ Interfaces ------
// ------------------------------------------------------------------------------------

/// @brief Detect sense board state changes and debounce 'em if exist
class SenseBoardStateDebouncer
{
	SenseBoardState _prev;
	uint32_t _time; ///< Timestamp when _prev was obtained
public:
	SenseBoardStateDebouncer() = default;

	bool tick(const SenseBoardState& newState) {
		uint32_t curTime = millis();
		if (curTime - _time < SENSEBOARD_DEBOUNCE_TIME_MS) // debounce time has not still passed
			return false;
		_time = curTime;
		if (newState == _prev) // state hasn't changed
			return false;
		_prev = newState;
		return true;
	}

	const SenseBoardState& getPrev() const { return _prev; }
};

/// @brief GameStateResolverInfo. A helper class that aggregates game states resolution information
struct GSResolverInfo {
	bool isInvalid = false;
	bool isIntermediate = false;
	bool isFinished = true; // state can be intermediate and finished at the same time
};

class GSResolver {
	const ChessRulesEngine* _rules = nullptr;
	GSTree _tree;
	GSResolverInfo _info;
public:
	GSResolver() = default;

	bool init(const ChessRulesEngine& rules, const ChessGameState& initState) {
		_rules = &rules;
		if (!_rules)
			return false;
		if (!_tree.init(initState))
			return false;
		return true;
	}

	const GSResolverInfo& update(const std::vector<ChessPieceLocation>& changes) {
		// TODO: validate changes for each head
		uint8_t branchCount = _tree.getBranchCount();
		for (uint8_t i = 0;; ++i) {
			const GSBranch* branch = _tree.getBranch(i);
			if (!branch)
				break;
		}
		// TODO: prune subtree
		return _info;
	}

	// /// @brief Checks if there's an uncertainty in the current gamestate (i.e. if there's a single branch for evaluation)
	// bool IsCurrentStateUnique() const {
	// 	// TODO: implement
	// 	return true;
	// }

	// /// @brief Checks if current state is invalid and not final in terms of game engine and next turn is necessary to be a valid state
	// /// @note Returns true even even if current state is final but can still be considered as intermediate
	// bool IsCurrentStateIntermediate() const {
	// 	// TODO: implement
	// 	return true;
	// }

	// bool IsCurrentStateValid() const {

	// }

	ChessGameState getGameState(uint8_t idx = 0) const {
		const GSBranch* branch = _tree.getBranch(idx);
		if (!branch)
			return ChessGameState::getUndefinedState();
		return branch->evaluateGameState();
	}
};

#endif // STATERESOLVER_H__