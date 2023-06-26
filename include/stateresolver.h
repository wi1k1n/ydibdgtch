#ifndef STATERESOLVER_H__
#define STATERESOLVER_H__

#include "rulesengine.h"
#include "senseboard.h"

#include <unordered_set>
#include <list>

// ------------------------------------------------------------------------------------
// ------ Data structure ------
// ------------------------------------------------------------------------------------

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

enum class CGSRInfo {

};

/// @brief A helper class that aggregates game states resolution information
struct ChessGameStatesResolverInfo {

};

class ChessGameStatesResolver {
	Tree<ChessGameState> _tree;
	const ChessRulesEngine* _rules = nullptr;
	ChessGameStatesResolverInfo _info;
public:
	ChessGameStatesResolver() = default;

	bool init(const ChessRulesEngine& rules) {
		_rules = &rules;
		return true;
	}

	const ChessGameStatesResolverInfo& update(const std::vector<ChessPieceLocation>& changes) {
		// TODO: validate changes for each head
		// TODO: prune subtree
	}

	/// @brief Checks if there's an uncertainty in the current gamestate (i.e. if there's a single branch for evaluation)
	bool IsCurrentStateUnique() const {
		// TODO: implement
		return true;
	}

	/// @brief Checks if current state is invalid and not final in terms of game engine and next turn is necessary to be a valid state
	/// @note Returns true even even if current state is final but can still be considered as intermediate
	bool IsCurrentStateIntermediate() const {
		// TODO: implement
		return true;
	}

	bool IsCurrentStateValid() const {

	}
};

#endif // STATERESOLVER_H__