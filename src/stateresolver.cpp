#include "stateresolver.h"

bool GSTree::init(const ChessGameState& initState) {
	_initState = initState;
	_branches.push_back(GSBranch(this));
	return true;
}

const ChessGameState& GSTree::getInitialState() const {
	return _initState;
}

uint8_t GSTree::getBranchCount() const {
	return _branches.size();
}

const GSBranch* GSTree::getBranch(uint8_t idx) const {
	// TODO: unify with getBranch*
	// DLOGLN(idx);
	if (idx >= _branches.size())
		return nullptr;
	const GSBranch& b = _branches[idx];
	// DLOGLN(b.toString());
	return &_branches[idx];
}
GSBranch* GSTree::getBranch(uint8_t idx) {
	// DLOGLN(idx);
	if (idx >= _branches.size())
		return nullptr;
	const GSBranch& b = _branches[idx];
	// DLOGLN(b.toString());
	return &_branches[idx];
}


GSBranch::GSBranch(GSTree* parent) : _parentTree(parent) {
	if (!parent)
		DLOGLN("Invalid GSBranch creation!"_f);
}

ChessGameState GSBranch::evaluateGameState() const {
	ChessGameState state = _parentTree->getInitialState();
	for (const auto& move : _moves) {
		switch (move.type) {
			case GSBoardMoveType::DISAPPEARED: {
				state.unset(move.pos);
				break;
			}
			case GSBoardMoveType::BACKONBOARD: {
				state.set(move.pos, move.piece);
				break;
			}
			default: break;
		}
		// if (!state.makeMove(move)) {
		// 	DLOG("Invalid move in branch "_f);
		// 	LOG(toString());
		// 	LOG(": "_f);
		// 	LOG(move.first.toString());
		// 	LOG(" -> "_f);
		// 	LOGLN(move.second.toString());
		// 	break;
		// }
	}
	return state;
}

bool GSBranch::disappeared(const ChessPieceLocation& pos) {
	// TODO: check for existance!
	_moves.push_back({GSBoardMoveType::DISAPPEARED, pos, evaluateGameState().at(pos)});
	return true;
}

bool GSBranch::backonboard(const ChessPieceLocation& pos) {
	if (_moves.empty())
		return false;
	GSBoardMove lastDisappearedMove = _moves.back();
	if (lastDisappearedMove.type != GSBoardMoveType::DISAPPEARED)
		return false;
	
	_moves.push_back({GSBoardMoveType::BACKONBOARD, pos, lastDisappearedMove.piece});
	return true;
}

String GSBranch::toString() const {
	return String(reinterpret_cast<int>(_parentTree));
}





bool GSResolver::init(const ChessRulesEngine& rules, const ChessGameState& initState) {
	_rules = &rules;
	if (!_rules)
		return false;
	if (!_tree.init(initState))
		return false;
	return true;
}

ChessGameState GSResolver::getGameState(uint8_t idx) const {
	const GSBranch* branch = _tree.getBranch(idx);
	if (!branch)
		return ChessGameState::getUndefinedState();
	return branch->evaluateGameState();
}

const GSResolverInfo& GSResolver::update(const std::vector<ChessPieceLocation>& changes) {
	// DLOGLN("update()"_f);
	if (changes.empty())
		return _info;
	
	// // TODO: validate changes for each branch
	// for (uint8_t i = 0;; ++i) {
	// 	const GSBranch* branch = _tree.getBranch(i);
	// 	if (!branch)
	// 		break;
	// }
	
	// DLOGLN(changes.size());
	// TODO: Start with an atomic changes
	if (changes.size() > 1)
		return invalidateInfo();

	ChessPieceLocation pos = changes.at(0);
	// DLOGLN(pos.toString());

	// TODO: Start with a single branch scenario
	GSBranch* branch = _tree.getBranch(0);
	if (!branch)
		return invalidateInfo();
	// DLOGLN(branch->toString());
	
	ChessGameState state = getGameState(0);
	// DLOGLN(state.toFEN());
	if (state.isUndefined())
		return invalidateInfo();
	
	ChessPiece piece = state.at(pos);
	// LOG(piece.toString());
	// LOG(" at "_f);
	// LOGLN(pos.toString());
	if (piece.isValid()) { // piece disappeared from the board
		branch->disappeared(pos);
		_info = {1, 1, 0}; // TODO: check for right color to move!
		return _info;
	}

	// piece appeared on the board
	branch->backonboard(pos);
	_info = {0, 1, 1}; // TODO: check for valid move, for right color
	

	// TODO: prune subtree
	return _info;
}