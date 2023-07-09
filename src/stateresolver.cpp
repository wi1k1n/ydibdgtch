#include "stateresolver.h"

// bool GSTree::init(const ChessGameState& initState) {
// 	_initState = initState;
// 	_branches.clear();
// 	_branches.push_back(GSBranch(this));
// 	return true;
// }

// const ChessGameState& GSTree::getInitialState() const {
// 	return _initState;
// }

// uint8_t GSTree::getBranchCount() const {
// 	return _branches.size();
// }

// const GSBranch* GSTree::getBranch(uint8_t idx) const {
// 	// TODO: unify with getBranch*
// 	// DLOGLN(idx);
// 	if (idx >= _branches.size())
// 		return nullptr;
// 	const GSBranch& b = _branches[idx];
// 	// DLOGLN(b.toString());
// 	return &_branches[idx];
// }
// GSBranch* GSTree::getBranch(uint8_t idx) {
// 	// DLOGLN(idx);
// 	if (idx >= _branches.size())
// 		return nullptr;
// 	const GSBranch& b = _branches[idx];
// 	// DLOGLN(b.toString());
// 	return &_branches[idx];
// }


// GSBranch::GSBranch(GSTree* parent) : _parentTree(parent) {
// 	if (!parent)
// 		DLOGLN("Invalid GSBranch creation!"_f);
// }

// ChessGameState GSBranch::evaluateGameState() const {
// 	ChessGameState state = _parentTree->getInitialState();
// 	for (const auto& move : _moves) {
// 		switch (move.type) {
// 			case GSBoardMoveType::DISAPPEARED: {
// 				state.unset(move.pos);
// 				break;
// 			}
// 			case GSBoardMoveType::BACKONBOARD: {
// 				state.set(move.pos, move.piece);
// 				break;
// 			}
// 			default: break;
// 		}
// 		// if (!state.makeMove(move)) {
// 		// 	DLOG("Invalid move in branch "_f);
// 		// 	LOG(toString());
// 		// 	LOG(": "_f);
// 		// 	LOG(move.first.toString());
// 		// 	LOG(" -> "_f);
// 		// 	LOGLN(move.second.toString());
// 		// 	break;
// 		// }
// 	}
// 	return state;
// }

// bool GSBranch::disappeared(const ChessPieceLocation& pos) {
// 	// TODO: check for existance!
// 	_moves.push_back({GSBoardMoveType::DISAPPEARED, pos, evaluateGameState().at(pos)});
// 	return true;
// }

// bool GSBranch::backonboard(const ChessPieceLocation& pos) {
// 	if (_moves.empty())
// 		return false;
// 	GSBoardMove lastDisappearedMove = _moves.back();
// 	if (lastDisappearedMove.type != GSBoardMoveType::DISAPPEARED)
// 		return false;
	
// 	_moves.push_back({GSBoardMoveType::BACKONBOARD, pos, lastDisappearedMove.piece});
// 	return true;
// }

// String GSBranch::toString() const {
// 	return String(reinterpret_cast<int>(_parentTree));
// }



GSNode::GSNode(GSResolver* resolver, GSNode* parent, uint16_t branchingMove, const ChessGameState& init)
: _resolver(resolver), _parent(parent), _branchingMove(branchingMove), _initialState(init) {
}

GSNode::GSNode(const GSNode &other)
: _resolver(other._resolver), _parent(other._parent), _branchingMove(other._branchingMove), _initialState(other._initialState) {
	_children.reserve(other._children.size());
	for (const auto &child : other._children)
		_children.push_back(child);
}

GSNode::~GSNode() {
	for (auto& child : _children)
		delete child;
}

void GSNode::take(ChessPieceLocation pos) {
	LOG("GSN-"_f); LOG(String(reinterpret_cast<uint32_t>(this), HEX)); LOG(" "_f);

	ChessGameState state = evaluateGameState();
	ChessPiece piece;
	if (!piece.isValid()) {
		LOG("! Take operation: no piece at "_f); LOGLN(pos.toString());
		return;
	}
	_buffer.push_back(piece);
	LOG("taken piece "_f); LOG(piece.toString()); LOG(" from "_f); LOGLN(pos.toString());
}

void GSNode::put(ChessPieceLocation pos) {
	LOG("GSN-"_f); LOG(String(reinterpret_cast<uint32_t>(this), HEX)); LOG(" "_f);
	ChessGameState state = evaluateGameState();
	ChessPiece piece;
	if (piece.isValid()) {
		LOG("! Put operation: there's a piece "_f); LOG(piece.toString()); LOG(" at "_f); LOGLN(pos.toString());
		return;
	}
	if (!_buffer.size()) {
		LOG("! Put operation: _buffer's empty "_f);
		return;
	}
	_buffer.pop_back();
}

ChessGameState GSNode::evaluateGameState() const {
	ChessGameState currentState = _initialState;
	const std::vector<ChessPieceLocation>& moves = _resolver->getMoves();
	std::vector<ChessPiece> buffer(2);
	DLOG("moves #"_f);
	LOGLN(moves.size());
	for (uint16_t idx = _branchingMove; idx < moves.size(); ++idx) {
		ChessPieceLocation curMovePos = moves[idx];
		ChessPiece piece = currentState.at(curMovePos);
		if (piece.isValid()) { // piece disappeared from the board
			buffer.push_back(piece);
			currentState.unset(curMovePos);
			continue;
		}
		// piece appeared on the board
		if (buffer.size() > 1) // TODO: no takes for now
			DLOGLN("buffer size > 1");
		currentState.set(curMovePos, buffer[0]);
		buffer.pop_back();
	}
	return currentState;
}


bool GSResolver::init(const ChessRulesEngine& rules, const ChessGameState& initState) {
	_rules = &rules;
	if (!_rules)
		return false;
	_moveLocations.clear();
	_root = GSNode(this, nullptr, 0, initState);
	_heads.clear();
	_heads.push_back(&_root);
	return true;
}

// TODO: do some caching here!
ChessGameState GSResolver::getGameState(uint8_t idx) const {
	// const GSBranch* branch = _tree.getBranch(idx);
	// if (!branch)
	// 	return ChessGameState::getUndefinedState();
	// return evaluateGameState();
	if (_heads.size() != 1) {
		DLOGLN("Heads size is supposed to be 1 for now!"_f);
		return ChessGameState::getUndefinedState();
	}
	GSNode* head = _heads[0];
	if (!head) {
		DLOGLN("Head is nullptr!"_f);
		return ChessGameState::getUndefinedState();
	}
	return head->evaluateGameState();
}

const bool GSResolver::update(const std::vector<ChessPieceLocation>& changes) {
	// DLOGLN("update()"_f);
	if (changes.empty())
		return true;
	
	// for (uint8_t i = 0;; ++i) {
	// 	const GSBranch* branch = _tree.getBranch(i);
	// 	if (!branch)
	// 		break;
	// }

	// TODO: start with a single branch, should be able to validate changes for all branches
	if (_heads.size() > 1) { LOGLN("More than 1 branches aren't supported yet"_f); return true; }
	
	// TODO: Start with an atomic changes, should be able to process complex as well
	if (changes.size() > 1) { LOGLN("More than 1 change isn't supported yet"_f); return true; }

	ChessPieceLocation pos = changes.at(0);	
	ChessGameState state = getGameState(0);
	ChessPiece piece = state.at(pos);

	// LOG(piece.toString());
	// LOG(" at "_f);
	// LOGLN(pos.toString());

	GSNode* curBranch = _heads[0];
	if (!curBranch) { DLOGLN("Unexpected nullptr in current branch!"_f); return false; }

	if (piece.isValid()) { // piece disappeared from the board
		curBranch->take(pos);
		return true;
	}

	// piece appeared on the board
	curBranch->put(pos);
	

	// TODO: prune subtree
	return true;
}