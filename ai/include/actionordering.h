#ifndef ACTIONORDERING_H
#define ACTIONORDERING_H

#include "gamestate.h"
#include "stateanalysis.h"
#include "action.h"
#include "staticvector.h"
#include <utility>
#include <algorithm>

struct ActionOrdering {

    ActionSet* const actions;
    static_vector<double, 64> scores;

    template<typename GameState>
    ActionOrdering(ActionSet* actions, const GameState& state) :
        actions(actions)
    {
        StateAnalysis analyzer(state.board, state.reserve0, state.reserve1);
        scores.clear();
        scores.reserve(actions->size());
        for(const Action& action : *actions) {
            if(action.p.color() == P0) scores.push_back(score0(action, state, analyzer));
            if(action.p.color() == P1) scores.push_back(score1(action, state, analyzer));
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

    static constexpr std::array<double, NB_PIECE_TYPE> pieceValue {
        0.0,     // NoType,
        1000.0,  // King,
        50.0,    // Rook,
        50.0,    // Bishop,
        10.0,    // Pawn,
        30.0,    // Queen,
    };

    template<typename GameState>
    static double score0(const Action action, const GameState& state, const StateAnalysis& analyzer) {
        double score = 0;
        if(analyzer.controlled1[action.dst.idx()]) {
            double val = pieceValue[action.p.type()];
            score -= val;
            if(!analyzer.controlled0[action.dst.idx()]) score -= val;
        }
        if(analyzer.occupied1[action.dst.idx()]) score += pieceValue[state.board.get(action.dst.idx()).type()];
        return score;
    }

    template<typename GameState>
    static double score1(const Action action, const GameState& state, const StateAnalysis& analyzer) {
        double score = 0;
        if(analyzer.controlled0[action.dst.idx()]) {
            double val = pieceValue[action.p.type()];
            score -= val;
            if(!analyzer.controlled1[action.dst.idx()]) score -= val;
        }
        if(analyzer.occupied0[action.dst.idx()]) score += pieceValue[state.board.get(action.dst.idx()).type()];
        return score;
    }

};


#endif
