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
	DLOGLN(idx);
	if (idx >= _branches.size())
		return nullptr;
	const GSBranch& b = _branches[idx];
	DLOGLN(b.toString());
	return &_branches[idx];
}


GSBranch::GSBranch(GSTree* parent) : _parentTree(parent) {
	if (!parent)
		DLOGLN("Invalid GSBranch creation!"_f);
}

ChessGameState GSBranch::evaluateGameState() const {
	// ChessGameState state = _parentTree->getInitialState();
	return _parentTree->getInitialState();
}

String GSBranch::toString() const {
	return String(reinterpret_cast<int>(_parentTree));
}