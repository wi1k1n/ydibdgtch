#pragma once

#include <chs_types.h>

#include "rulesengine.h"
#include "stateresolver.h"
#include "senseboard.h"

struct CGSState {
	CGSState() = default;
	CGSState(const ChessGameState& state, Array<ChessPiece> piecesInHand = Array<ChessPiece>())
		: state(state), piecesInHand(piecesInHand) {}

	bool operator==(const CGSState& other) const {
		return state == other.state && piecesInHand == other.piecesInHand;
	}

	ChessGameState state;
	Array<ChessPiece> piecesInHand;
};

template<> struct std::hash<CGSState> {
	size_t operator()(const CGSState& other) const {
		return hash<ChessGameState>()(other.state);
	}
};

class CGSNode {
public:
	CGSNode() = default;
	CGSNode(const CGSState& state, CGSNode* parent)
		: m_state(state), m_parent(parent) {}

	const CGSState& GetState() const { return m_state; }
	Array<CGSNode*> GetChildren() const {
		Array<CGSNode*> children;
		for (const auto& child : m_children)
			children.push_back(child.get());
		return children;
	}
	CGSNode* GetParent() const { return m_parent; }

	void AddChild(CGSNode&& child) {
		child.m_parent = this;
		m_children.push_back(MakeUniquePtr<CGSNode>(Move(child)));
	}
	bool RemoveChild(CGSNode* child) {
		auto it = std::find_if(m_children.begin(), m_children.end(), [&](const auto& c) { return c.get() == child; });
		if (it == m_children.end())
			return false;
		m_children.erase(it);
		return true;
	}

	//bool operator==(const CGSNode& other) const {
	//	return m_state == other.m_state && m_children == other.m_children;
	//}

private:
	CGSState m_state;
	Array<UniquePtr<CGSNode>> m_children;
	CGSNode* m_parent = nullptr;
};

//template<> struct std::hash<CGSNode> {
//	size_t operator()(const CGSNode& other) const {
//		return std::hash<CGSState>()(other.GetState()) ^ std::hash<size_t>()(reinterpret_cast<size_t>(other.GetParent()));
//	}
//};

class CGSGraph {
public:
	CGSGraph() = default;

	void Init(const ChessGameState& initState) {
		m_root = MakeUniquePtr<CGSNode>(CGSNode(initState, m_root.get()));
	}

	CGSNode* GetRoot() const { return m_root.get(); }

	void AddNode(CGSNode&& node, CGSNode* parent);
	bool RemoveNode(CGSNode* node);
	bool RemoveNodes(const Array<CGSNode*>& nodes);

	void GetNodesAtLevel(uint16_t level, Array<CGSNode*>& outNodes) const;
	void Traverse(int startDepthLevel, Function<bool(CGSNode*, int)> callback) const;

private:
	UniquePtr<CGSNode> m_root;
};

class StateResolver {
public:
	StateResolver() = default;

	void Init(const ChessGameState& initState, const SenseBoardState& boardState);

	void UpdateBoardState(const SenseBoardState& boardState);
	void GetPossibleGamestates(Array<CGSState>& outStates);

private:
	Array<SenseBoardState> m_boardStates;
	CGSGraph m_graph;
};