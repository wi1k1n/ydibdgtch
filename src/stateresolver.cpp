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

static std::vector<ChessMoveLocation> getValidMovesResultingInTakingPiece(const ChessGameState& state, ChessPieceLocation pos, const ChessRulesEngine& rules) {
	std::vector<ChessMoveLocation> moves;
	ChessPiece chessPiece = state.at(pos);
	CHESSPIECE piece = chessPiece.getPiece();
	CHESSCOLOR clr = chessPiece.getColor();
	CHESSCOLOR clrOpposite = chessPiece.getColorOpposite();
	if (piece == CHESSPIECE::UNKNOWN || clr == CHESSCOLOR::UNKNOWN) {
		return moves;
	}
	const std::unordered_map<ChessPieceLocation, ChessPiece>& pieces = state.getPieces();
	for (const auto& pieceEntry : pieces) { // for each piece of the opposite color
		if (pieceEntry.second.getColor() != clrOpposite)
			continue;
		std::vector<ChessMoveLocation> curMoves = rules.getValidMovesForPiece(state, pieceEntry.first);
		for (const auto& m : curMoves) { // collect taking moves that result in given pos
			if (static_cast<ChessPieceLocation>(m) == pos && m.isTaking())
				moves.push_back(m);
		}
	}
	return moves;
}

bool  GSNode::update(ChessPieceLocation pos) {
	LOG("GSN-"_f); LOG(String(reinterpret_cast<uint32_t>(this), HEX)); LOG(" "_f);

	if (!_resolver) {
		DLOGLN("_resolver is nulltpr!"_f);
		return false;
	}
	const ChessRulesEngine* enginePtr = _resolver->getRulesEngine();
	if (!enginePtr) {
		DLOGLN("rules enginePtr is nullptr!"_f);
		return false;
	}
	const ChessRulesEngine& engine = *enginePtr;
	ChessGameState state = evaluateGameState();
	CHESSCOLOR colorToMove = state.getColorToMove();
	if (colorToMove == CHESSCOLOR::UNKNOWN) {
		DLOGLN("color to move is undefined, cannot validate moves!"_f);
	}
	ChessPiece piece = state.at(pos);

	// ------------------------------
	// Piece is there, take operation
	if (piece.isValid()) {
		if (piece.getColor() == colorToMove) { // taken piece of the same color
			std::vector<ChessMoveLocation> validMoves = engine.getValidMovesForPiece(state, pos);
			if (validMoves.empty()) {
				DLOGLN("this piece doesn't have valid moves and shouldn't be taken"_f);
				return false;
			}
		} else { // taken piece of the opposite color
			std::vector<ChessMoveLocation> validTakeMoves = getValidMovesResultingInTakingPiece(state, pos, engine);
			if (validTakeMoves.empty()) {
				DLOGLN("this piece cannot be taken and shouldn't be taken"_f);
				return false;
			}
		}
		_buffer.push_back(std::make_tuple(pos, piece));
		LOG("taken piece "_f); LOG(piece.toString()); LOG(" from "_f); LOGLN(pos.toString());
		return true;
	}

	// ------------------------------
	// No piece at pos, put operation
	if (_buffer.empty()) {
		DLOGLN("buffer is empty! Couldn't make put operation!"_f);
		return false;
	}
	// if (_buffer.size() > 1) {
	// 	DLOGLN("more than 1 piece in buffer! Need rules resolution!"_f);
	// }
	const std::vector<ChessPieceLocation>& moves = _resolver->getMoves();
	if (moves.empty()) {
		DLOGLN("moves is empty, didn't expect this!"_f);
		return false;
	}

	if (_buffer.size() == 1) { // single piece in buffer, regular piece movement
		// Simulate this move in a separate game state
		ChessPieceLocation bufPieceFrom; ChessPiece bufPiece;
		std::tie(bufPieceFrom, bufPiece) = _buffer.front();
		
		ChessGameState curState = state;
		curState.set(bufPieceFrom, bufPiece);
		std::vector<ChessMoveLocation> validMoves = engine.getValidMovesForPiece(curState, bufPieceFrom);
		bool validMoveFound = false;
		for (const auto& curMove : validMoves) {
			if (static_cast<ChessPieceLocation>(curMove) == pos && !curMove.isTaking()) {
				validMoveFound = true;
				break;
			}
		}
		if (!validMoveFound) {
			DLOGLN("no valid move found! (could be intermediate?)"_f);
			return false;
		}
		// valid move found, switch turn
	}

	LOG("put piece "_f); LOG(std::get<1>(_buffer[_buffer.size() - 1]).toString()); LOG(" to "_f); LOGLN(pos.toString());
	_buffer.pop_back();
	return true;
}

ChessGameState GSNode::evaluateGameState() const {
	ChessGameState currentState = _initialState;
	const std::vector<ChessPieceLocation>& moves = _resolver->getMoves();
	std::vector<ChessPiece> buffer;
	// DLOG("moves #"_f);
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
		if (!buffer.size()) {
			DLOGLN("buffer is empty!"_f);
			continue;
		}
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
	// TODO: Start with an atomic changes, should be able to process complex as well
	if (changes.size() > 1) { LOGLN("More than 1 change isn't supported yet"_f); return true; }
	
	// TODO: start with a single branch, should be able to validate changes for all branches
	if (_heads.size() > 1) { LOGLN("More than 1 branches aren't supported yet"_f); return true; }

	ChessPieceLocation pos = changes.at(0);	

	GSNode* curBranch = _heads[0];
	if (!curBranch) { DLOGLN("Unexpected nullptr in current branch!"_f); return false; }
	curBranch->update(pos);

	_moveLocations.push_back(pos);
	
	// TODO: prune subtree
	return true;
}