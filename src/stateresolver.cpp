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



GSNode::GSNode(GSResolver* resolver, GSNode* parent, const ChessGameState& init)
: _resolver(resolver), _parent(parent), _state(init) {
}

GSNode::GSNode(const GSNode &other)
: _resolver(other._resolver), _parent(other._parent), _state(other._state) {
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

bool GSNode::update(ChessPieceLocation pos) {
	LOG("GSN-"_f); LOG(String(reinterpret_cast<uint32_t>(this), HEX)); LOG(" "_f);
	if (!_resolver) { DLOGLN("_resolver is nulltpr!"_f); return false; }
	const ChessRulesEngine* engine = _resolver->getRulesEngine();
	if (!engine) { DLOGLN("rules enginePtr is nullptr!"_f); return false; }

	CHESSCOLOR colorToMove = _state.getColorToMove();
	if (colorToMove == CHESSCOLOR::UNKNOWN) { DLOGLN("color to move is undefined, cannot validate moves!"_f); }
	
	ChessPiece piece = _state.at(pos);

	// ------------------------------
	// Piece is there, take operation
	if (piece.isValid()) {
		if (_buffer.size() >= 2) { DLOGLN("INVALID MOVE! No case requires more than 2 pieces being taken!"_f); return false; }

		// if there's a single piece in buffer, it's either taking or castling
		if (_buffer.size() == 1) {
			ChessPiece bufPiece = _buffer.front().second;
			if (bufPiece.getColor() == piece.getColor()) { // same color -> only castling!
				if (piece.getColorOpposite() == colorToMove) { DLOGLN("INVALID CASE: it is neight castling nor taking!"_f); return false; }
				LOGLN("=> It's castling!.. need to check pieces though"_f);
			} else { // opposite colors -> taking
				LOGLN("=> It's taking!.. need to check pieces though"_f);
			}
		}
		// if there's no piece in buffer, it can be anything (:

		if (piece.getColor() == colorToMove) { // taken piece of the same color -> can be moving, taking or castling
			LOGLN("=> It's taking/moving/castling!.. need to check pieces though"_f);
		} else { // taken piece of the opposite color
			LOGLN("=> It's taking (or continuation of the intermediate move)!.. need to check pieces though"_f);
		}
		_buffer.push_back(std::make_pair(pos, piece));
		_state.unset(pos);
		LOG("taken piece "_f); LOG(piece.toString()); LOG(" from "_f); LOGLN(pos.toString());
		return true;
	}

	// ------------------------------
	// No piece at pos, put operation
	if (_buffer.empty() || _buffer.size() > 2) { DLOGLN("INVALID: buffer is empty or > 2! Couldn't make put operation!"_f); return false; }

	// 1 piece in buffer -> finishing simple move
	if (_buffer.size() == 1) {
		ChessPiece pieceBuf;
		ChessPieceLocation posBuf;
		std::tie(posBuf, pieceBuf) = _buffer.front();

		LOGLN("=> It's simple move!.. need to check pieces though"_f);
		LOG("put piece "_f); LOG(pieceBuf.toString()); LOG(" from "_f); LOG(posBuf.toString()); LOG(" to "_f); LOGLN(pos.toString());
		_state.set(pos, pieceBuf);
		_state.nextTurn();
		_buffer.clear();
		return true;
	}

	// 2 pieces in buffer -> finishing taking or castling in progress
	ChessPiece pieceFirst, pieceSecond;
	ChessPieceLocation posFirst, posSecond;
	std::tie(posFirst, pieceFirst) = _buffer.front();
	std::tie(posSecond, pieceSecond) = _buffer.back();

	if (pieceFirst.getColor() == pieceSecond.getColor()) { // same color -> castling
		if (pieceFirst.getColorOpposite() == colorToMove) { DLOGLN("INVALID: it's supposed to be castling, but wrong color!"_f); return false; }
		LOGLN("=> It's castling in progress!.. need to check pieces though"_f);
		// ...
	} else { // different color -> finishing taking
		if (pieceFirst.getColor() == colorToMove) { // pieceFirst is the piece that's put back
			_state.set(pos, pieceFirst);
			LOG("put piece "_f); LOG(pieceFirst.toString()); LOG(" from "_f); LOG(posFirst.toString()); LOG(" to "_f); LOGLN(pos.toString());
		} else { // pieceSecond is the piece that's put back
			_state.set(pos, pieceSecond);
			LOG("put piece "_f); LOG(pieceSecond.toString()); LOG(" from "_f); LOG(posSecond.toString()); LOG(" to "_f); LOGLN(pos.toString());
		}
		_state.nextTurn();
		_buffer.clear();
	}
	return true;

	// const std::vector<ChessPieceLocation>& moves = _resolver->getMoves();
	// if (moves.empty()) {
	// 	DLOGLN("moves is empty, didn't expect this!"_f);
	// 	return false;
	// }

	// if (_buffer.size() == 1) { // single piece in buffer, regular piece movement
	// 	// Simulate this move in a separate game state
	// 	ChessPieceLocation bufPieceFrom; ChessPiece bufPiece;
	// 	std::tie(bufPieceFrom, bufPiece) = _buffer.front();
		
	// 	ChessGameState curState = state;
	// 	curState.set(bufPieceFrom, bufPiece);
	// 	std::vector<ChessMoveLocation> validMoves = engine.getValidMovesForPiece(curState, bufPieceFrom);
	// 	bool validMoveFound = false;
	// 	for (const auto& curMove : validMoves) {
	// 		if (static_cast<ChessPieceLocation>(curMove) == pos && !curMove.isTaking()) {
	// 			validMoveFound = true;
	// 			break;
	// 		}
	// 	}
	// 	if (!validMoveFound) {
	// 		DLOGLN("no valid move found! (could be intermediate?)"_f);
	// 		return false;
	// 	}
	// 	// valid move found, switch turn
	// }

	// LOG("put piece "_f); LOG(_buffer[_buffer.size() - 1].second.toString()); LOG(" to "_f); LOGLN(pos.toString());
	// _buffer.pop_back();
	// return true;
}

// ChessGameState GSNode::evaluateGameState() const {
// 	ChessGameState currentState = _initialState;
// 	CHESSCOLOR colorToMove = currentState.getColorToMove();
// 	if (colorToMove == CHESSCOLOR::UNKNOWN) {
// 		DLOGLN("color to move is undefined, cannot validate moves!"_f);
// 		return ChessGameState::getUndefinedState();
// 	}
	
// 	const std::vector<ChessPieceLocation>& moves = _resolver->getMoves();
// 	if (_moveLocationsInfo.size() != moves.size()) {
// 		DLOGLN("invalid _moveLocationsInfo size!"_f);
// 		return ChessGameState::getUndefinedState();
// 	}

// 	std::vector<std::pair<ChessPieceLocation, ChessPiece>> buffer;
// 	// DLOG("moves #"_f); LOGLN(moves.size());

// 	for (uint16_t idx = _branchingMove; idx < moves.size(); ++idx) {
// 		ChessPieceLocation curMovePos = moves[idx];
// 		ChessPiece piece = currentState.at(curMovePos);
// 		const AdditionalMoveData& data = _moveLocationsInfo[idx];

// 		// piece disappeared from the board
// 		if (piece.isValid()) {
// 			buffer.push_back(std::make_pair(curMovePos, piece));
// 			currentState.unset(curMovePos);
// 			continue;
// 		}

// 		// piece appeared on the board
// 		if (!buffer.size()) {
// 			DLOGLN("INVALID: buffer is empty!"_f);
// 			continue;
// 		}
// 		if (buffer.size() > 1) // TODO: cannot resolve here takes for now
// 			DLOGLN("buffer size > 1");
// 		currentState.set(curMovePos, buffer[0].second);
// 		buffer.pop_back();
// 	}
// 	return currentState;
// }


bool GSResolver::init(const ChessRulesEngine& rules, const ChessGameState& initState) {
	_rules = &rules;
	if (!_rules)
		return false;
	_moveLocations.clear();
	_root = GSNode(this, nullptr, initState);
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
	if (_heads.size() != 1) { DLOGLN("Heads size is supposed to be 1 for now!"_f); return ChessGameState::getUndefinedState(); }
	GSNode* head = _heads[0];
	if (!head) { DLOGLN("Head is nullptr!"_f); return ChessGameState::getUndefinedState(); }

	return head->getGameState();
}

const bool GSResolver::update(const std::vector<ChessPieceLocation>& changes) {
	// DLOGLN("update()"_f);
	if (changes.empty()) return true;
	
	// TODO: Start with an atomic changes, should be able to process complex as well
	if (changes.size() > 1) { LOGLN("More than 1 change isn't supported yet"_f); return true; }
	
	// TODO: start with a single branch, should be able to validate changes for all branches
	if (_heads.size() > 1) { LOGLN("More than 1 branches aren't supported yet"_f); return true; }

	ChessPieceLocation pos = changes.at(0);	
	if (!pos.isOnBoard()) { DLOGLN("SANITY FAILED: change pos is not on board"); return true; }

	GSNode* curBranch = _heads[0];
	if (!curBranch) { DLOGLN("Unexpected nullptr in current branch!"_f); return false; }
	
	curBranch->update(pos);
	_moveLocations.push_back(pos);
	
	// TODO: prune subtree
	return true;
}