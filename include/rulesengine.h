#ifndef RULESENGINE_H__
#define RULESENGINE_H__

struct GameState {

};

class RulesEngine {
	virtual GameState getStartingState() const { return GameState(); }
	virtual bool isStateValid(const GameState& state) const { return true; }
	virtual bool isStateChangeValid(const GameState& state1, const GameState& state2) const { return true; }
};

class ChessRules : public RulesEngine {

};

class ClassicChessRules : public ChessRules {

};

#endif // RULESENGINE_H__