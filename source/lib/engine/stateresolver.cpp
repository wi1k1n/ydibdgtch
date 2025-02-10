#include "stateresolver.h"
#include "chs_common.h"


void CGSGraph::AddNode(CGSNode&& node, CGSNode* parent) {
	if (parent)
		parent->AddChild(Move(node));
	else
		m_root = MakeUniquePtr<CGSNode>(Move(node));
}

bool CGSGraph::RemoveNode(CGSNode* node) {
	if (node == m_root.get()) {
		m_root.reset();
		return true;
	}
	CGSNode* parent = nullptr;
	Traverse(0, [&](CGSNode* currentNode, int nodeLevel) {
		if (currentNode != node)
			return true;
		parent = currentNode->GetParent();
		return false;
	});
	return parent->RemoveChild(node);
}

void CGSGraph::GetNodesAtLevel(uint16_t level, Array<CGSNode*>& outNodes) const {
	outNodes.clear();
	Traverse(level, [&](CGSNode* node, int nodeLevel) {
		if (nodeLevel != level)
			return true;
		outNodes.push_back(node);
		return false;
	});
}

void CGSGraph::Traverse(int startDepthLevel, Function<bool(CGSNode*, int)> callback) const {
	Array<CGSNode*> nodes;
	Array<uint16_t> levels;
	nodes.push_back(m_root.get());
	levels.push_back(0);

	for (int cursorIdx = 0; cursorIdx < nodes.size(); cursorIdx++) {
		CGSNode* node = nodes[cursorIdx];
		const uint16_t nodeLevel = levels[cursorIdx];

		if (nodeLevel >= startDepthLevel)
			if (!callback(node, nodeLevel))
				continue;

		const Array<CGSNode*>& children = node->GetChildren();
		for (CGSNode* child : children) {
			nodes.push_back(child);
			levels.push_back(nodeLevel + 1);
		}
	}
}

//
//bool CGSGraph::RemoveNode(CGSNode& node) {
//	return RemoveNodes({ node });
//}
//
//bool CGSGraph::RemoveNodes(const Array<CGSNode>& nodes) {
//	//Hashset<CGSNode> nodesToRemove(nodes.begin(), nodes.end());
//	//TraverseBFS(0, [&](CGSNode& currentNode, int nodeLevel) {
//	//	if (nodesToRemove.empty())
//	//		return false;
//	//	
//	//	auto it = nodesToRemove.find(currentNode);
//	//	if (it != nodesToRemove.end())
//	//		return true;
//
//	//	if (CGSNode* parent = currentNode.GetParent())
//	//		parent->RemoveChild(currentNode);
//	//	nodesToRemove.erase(it);
//	//	return false;
//	//	});
//	//return nodesToRemove.empty();
//	return false;
//}
//
//void CGSGraph::GetNodesAtLevel(uint16_t level, Array<CGSNode>& outNodes) {
//	//outNodes.clear();
//	//TraverseBFS(0, [&](CGSNode& node, int nodeLevel) {
//	//		if (nodeLevel != level)
//	//			return true;
//	//		outNodes.push_back(node);
//	//		return false;
//	//	});
//}
//
//void CGSGraph::TraverseBFS(int level, Function<bool(CGSNode&, int)> callback) {
//	//Array<CGSNode> nodes;
//	//Array<uint16_t> levels;
//	//nodes.push_back(m_root);
//	//levels.push_back(0);
//
//	//for (int cursorIdx = 0; cursorIdx < nodes.size(); cursorIdx++) {
//	//	CGSNode& node = nodes[cursorIdx];
//	//	const uint16_t nodeLevel = levels[cursorIdx];
//
//	//	if (!callback(node, nodeLevel))
//	//		continue;
//
//	//	const Array<CGSNode>& children = node.GetChildren();
//	//	for (const CGSNode& child : children) {
//	//		nodes.push_back(child);
//	//		levels.push_back(nodeLevel + 1);
//	//	}
//	//}
//}


//void StateResolver::Init(const ChessGameState& initState, const SenseBoardState& boardState) {
//	m_graph.Init(initState);
//	m_boardStates.push_back(boardState);
//}
//
//void StateResolver::UpdateBoardState(const SenseBoardState& boardState) {
//	if (m_boardStates.empty()) {
//		LOGLN("StateResolver::UpdateBoardState called before Init!");
//		return;
//	}
//	const SenseBoardState& lastBoardState = m_boardStates[m_boardStates.size() - 1];
//	Array<CGSNode> lastLevelNodes;
//	m_graph.GetNodesAtLevel((uint16_t)(m_boardStates.size() - 1), lastLevelNodes);
//
//	Array<ChessPieceLocation> diffLocations = boardState - lastBoardState;
//	if (diffLocations.empty())
//		return;
//	if (diffLocations.size() > 1) {
//		LOGLN("Multiple piece moves detected (NOT SUPPORTED!");
//		return;
//	}
//
//	Array<int> invalidNodesIndices;
//
//	const ChessPieceLocation& pieceLocation = diffLocations[0];
//	const bool pieceRemoved = lastBoardState.Get(pieceLocation);
//
//	for (int nodeIdx = 0; nodeIdx < lastLevelNodes.size(); ++nodeIdx) {
//		CGSNode& node = lastLevelNodes[nodeIdx];
//		const CGSState& state = node.GetState();
//
//		const ChessGameState& gameState = state.state;
//		const Array<ChessPiece>& piecesInHand = state.piecesInHand;
//		const ChessPiece& piece = gameState.At(pieceLocation);
//
//		if (pieceRemoved != piece.IsValid()) {
//			LOGLN("Removed piece is not on the board, or added piece to the board, where it already is present");
//			invalidNodesIndices.push_back(nodeIdx);
//			continue;
//		}
//
//		// Piece was removed from the board
//		if (pieceRemoved) {
//			if (piecesInHand.size() > 0) {
//				LOGLN("Piece removed but pieces in hand detected (NOT SUPPORTED YET!)");
//				invalidNodesIndices.push_back(nodeIdx);
//				continue;
//			}
//
//			if (piece.GetColor() != gameState.GetColorToMove()) {
//				LOGLN("Piece removed but it's not the turn of the player who removed it!");
//				invalidNodesIndices.push_back(nodeIdx);
//				continue;
//			}
//
//			Array<ChessPiece> newPiecesInHand(piecesInHand);
//			newPiecesInHand.push_back(piece);
//
//			ChessGameState newGameState(gameState);
//			newGameState.Unset(pieceLocation);
//
//			CGSState newState(newGameState, newPiecesInHand);
//			CGSNode newNode(newState, &node);
//			m_graph.AddNode(newNode);
//			continue;
//		}
//
//		// Piece was added to the board
//		if (piecesInHand.empty()) {
//			LOGLN("Piece added to the board but no pieces in hand detected (NOT SUPPORTED YET!)");
//			invalidNodesIndices.push_back(nodeIdx);
//			continue;
//		}
//
//		const ChessPiece& pieceInHand = piecesInHand[piecesInHand.size() - 1];
//		if (pieceInHand.GetColor() != gameState.GetColorToMove()) {
//			LOGLN("Piece added to the board but it's not the turn of the player who added it!");
//			invalidNodesIndices.push_back(nodeIdx);
//			continue;
//		}
//
//		ChessGameState newGameState(gameState);
//		newGameState.Set(pieceLocation, pieceInHand);
//		newGameState.NextTurn();
//
//		CGSState newState(newGameState);
//		CGSNode newNode(newState, &node);
//		m_graph.AddNode(newNode);
//	}
//
//	m_boardStates.push_back(boardState);
//
//	Array<CGSNode> nodesToRemove;
//	for (int invalidNodeIdx : invalidNodesIndices)
//		nodesToRemove.push_back(lastLevelNodes[invalidNodeIdx]);
//	m_graph.RemoveNodes(nodesToRemove);
//}
//
//void StateResolver::GetPossibleGamestates(Array<CGSState>& outStates) {
//	outStates.clear();
//	Array<CGSNode> lastLevelNodes;
//	m_graph.GetNodesAtLevel((uint16_t)(m_boardStates.size() - 1), lastLevelNodes);
//	for (const CGSNode& node : lastLevelNodes)
//		outStates.push_back(node.GetState().state);
//}
