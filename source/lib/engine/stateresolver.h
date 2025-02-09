#pragma once

#include <chs_types.h>

#include "rulesengine.h"
#include "stateresolver.h"
#include "senseboard.h"

class CGSNode {
public:
	CGSNode() = default;

	CGSNode(const ChessGameState& state, CGSNode* parent = nullptr, const Array<CGSNode>& children = Array<CGSNode>())
		: m_state(state), m_parent(parent), m_children(children) {}

private:
	ChessGameState m_state;
	
	Array<CGSNode> m_children;
	CGSNode* m_parent = nullptr;
};

class CGSGraph {
public:
	CGSGraph() = default;

	void Init(const ChessGameState& initState) {
		m_root = initState;
	}

private:
	CGSNode m_root;
};

class State {
public:
	
};

class StateResolver {
public:
	StateResolver() = default;

	void Init(const ChessGameState& initState, const SenseBoardState& boardState) {
		//initState.
		//m_graph.Init(initState);
	}

private:
	Array<SenseBoardState> m_boardStates;
	CGSGraph m_graph;
};