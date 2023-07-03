#ifndef STATERESOLVER_H__
#define STATERESOLVER_H__

#include "rulesengine.h"
#include "senseboard.h"

#include <unordered_set>
#include <list>

// ------------------------------------------------------------------------------------
// ------ Data structure ------
// ------------------------------------------------------------------------------------

#if 0 // could be useful
/// @brief Node in a Tree data structure
/// @tparam T Payload type
template<typename T>
class Node
{
	T _payload;
	Node<T>* _parent = nullptr; // weak reference -> doesn't own the object
public:
	Node(const T& pl, Node<T>* parent) : _payload(pl), _parent(parent) { }
	const T& getValue() const { return _payload; }
	T& getValue() { return _payload; }

	Node* getParent() { return _parent; }
};

/// @brief A strange backward-tree, that has multiple heads (leaves) pointing to its parents
/// @tparam T Payload type
template<typename T>
class Tree
{
	std::list<Node<T>> _nodes;
	std::vector<Node<T>*> _heads;
public:
	Tree() = default;
};
#endif

/// @brief GameStateBranch
class GSBranch {

};

/// @brief GameStateTree. Main game states container interface
class GSTree {
	ChessGameState _initState;
public:
	bool init(const ChessGameState& initState) {
		_initState = initState;
		return true;
	}


	GSBranch* getBranch(uint8_t idx) {
		return nullptr;
	}

	uint8_t getBranchCount() const {
		return 1;
	}
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
			GSBranch* branch = _tree.getBranch(i);
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
};

#endif // STATERESOLVER_H__