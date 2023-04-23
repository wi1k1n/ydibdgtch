#ifndef STATERESOLVER_H__
#define STATERESOLVER_H__

#include "rulesengine.h"
#include "senseboard.h"

#include <unordered_set>

/// @brief Node in a Tree data structure
/// @tparam T Payload type
template<typename T>
class Node
{
	T _payload;
	Node* _parent = nullptr;
public:
	const T& getValue() const { return _payload; }
	T& getValue() { return _payload; }

	Node* getParent() { return _parent; }
};

/// @brief A strange backward-tree, that has multiple heads (leaves) pointing to its parents
/// @tparam T Payload type
template<typename T>
class Tree
{
	std::vector<Node<T>*> _heads;
public:
	Tree() = default;
};

/// @brief Detect sense board state changes and debounce 'em if exist
class SenseBoardStateDebouncer
{
	SenseBoardState _prev;
	uint32_t _time; ///< Timestamp when _prev was obtained
public:
	SenseBoardStateDebouncer() = default;

	bool tick(const SenseBoardState& newState) {
		uint32_t curTime = millis();
		if (curTime - _time < SENSEBOARD_DEBOUNCE_TIME_MS)
			return false;
		if (newState == _prev)
			return false;
		_prev = newState;
		_time = curTime;
		return true;
	}

	const SenseBoardState& getPrev() const { return _prev; }
};

class ChessGameStatesResolver {
	Tree<ChessGameState> _tree;
	const ChessRulesEngine* _rules = nullptr;
public:
	ChessGameStatesResolver() = default;

	bool init(const ChessRulesEngine& rules) {
		_rules = &rules;
		return true;
	}

	void update(const std::unordered_set<ChessPieceLocation>& changes) {
		// TODO: validate changes for each head
		// TODO: prune subtree
	}
};

#endif // STATERESOLVER_H__