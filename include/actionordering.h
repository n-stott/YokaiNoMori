#ifndef ACTIONORDERING_H
#define ACTIONORDERING_H

#include "gamestate.h"
#include "stateanalysis.h"
#include "action.h"
#include "resourcepool.h"
#include "staticvector.h"
#include <vector>
#include <utility>
#include <algorithm>


struct ActionOrdering {

    ActionSet* const actions;
    static_vector<double, 64> scores;

    ActionOrdering(ActionSet* actions, const GameState& state) :
        actions(actions)
    {
        StateAnalysis analyzer(state);
        scores.clear();
        scores.reserve(actions->size());
        for(const Action& action : *actions) {
            if(action.p.color() == P1) scores.push_back(score1(action, state, analyzer));
            if(action.p.color() == P2) scores.push_back(score2(action, state, analyzer));
        }
    }

    void sort() {
        ActionSet& actionset = *actions;
        static_vector<aspair, 64> actionScores;

        const size_t size = scores.size();
        actionScores.clear();
        actionScores.reserve(size);
        for(size_t i = 0; i < size; ++i) {
            actionScores.emplace_back(std::make_pair(actionset[i], scores[i]));
        }
        std::sort(actionScores.begin(), actionScores.end(), [](const aspair& a, const aspair& b) { return a.second > b.second; });
        for(size_t i = 0; i < size; ++i) {
            actionset[i] = actionScores[i].first;
        }
    }

private:

    static double score1(const Action& action, const GameState& state, const StateAnalysis& analyzer) {
        double score = 0;
        if(action.type == ActionType::Move) {
            if(action.p.type() == King && analyzer.controlled2[action.dst.idx()]) score -= 100;
        }
        if(!analyzer.controlled1[action.dst.idx()] && analyzer.controlled2[action.dst.idx()]) score -= 10;
        if(action.p.type() == Pawn && analyzer.occupied2[action.dst.idx()]) score += 10;
        if(analyzer.occupied2[action.dst.idx()] && state.board[action.dst.idx()].type() == King) score += 1000;
        return score;
    }

    static double score2(const Action& action, const GameState& state, const StateAnalysis& analyzer) {
        double score = 0;
        if(action.type == ActionType::Move) {
            if(action.p.type() == King && analyzer.controlled1[action.dst.idx()]) score -= 100;
        }
        if(!analyzer.controlled2[action.dst.idx()] && analyzer.controlled1[action.dst.idx()]) score -= 10;
        if(action.p.type() == Pawn && analyzer.occupied1[action.dst.idx()]) score += 10;
        if(analyzer.occupied1[action.dst.idx()] && state.board[action.dst.idx()].type() == King) score += 1000;
        return score;
    }

};


#endif