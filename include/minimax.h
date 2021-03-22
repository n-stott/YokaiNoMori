#ifndef SEARCH_H
#define SEARCH_H

#include "gamestate.h"
#include "actionordering.h"
#include "action.h"
#include "agent.h"
#include <limits>
#include <optional>
#include <iostream>

struct Minimax {

    int maxdepth;
    GameState& root;
    std::optional<Action> bestAction;
    double bestScore;
    Agent& agent;

    Minimax(GameState& root, Agent& agent, int maxdepth) :
        maxdepth(maxdepth),
        root(root),
        bestAction(),
        bestScore(-std::numeric_limits<double>::infinity()),
        agent(agent)
    { }

    enum Mode {
        PureMinimax,
        AlphaBeta
    };

    double run(Mode mode) {
        if(mode == PureMinimax) {
            return search(root, maxdepth);
        }
        if(mode == AlphaBeta) {
            double inf = std::numeric_limits<double>::infinity();
            return alphaBetaSearch(root, maxdepth, -inf, inf);
        }
        return 0.0;
    }

private:

    static constexpr double discount = 1; //0.98;

    double search(GameState currentState, int depth) {
        if(depth == 0) {
            Agent::score score = agent.evaluate(currentState);
            double eval = score.value();
            assert(eval == eval);
            return eval;
        }
        ActionSet actionset = currentState.allowedActions();
        ActionOrdering orderer(actionset, currentState);
        orderer.sort();
        if(actionset.empty()) {
            if(currentState.hasWon(currentState.currentPlayer)) {
                return -agent.kingDeadValue;
            }
            if(currentState.hasLost(currentState.currentPlayer)) {
                return agent.kingDeadValue;
            }
            return 0;
        }

        double bestEvaluation = -std::numeric_limits<double>::infinity();
        for(Action action : actionset) {
            GameState tmp = currentState;
            tmp.apply(action);
            double evaluation = -discount*search(tmp, depth-1);
            if(depth == maxdepth) {
                std::cout << action.toString() << " : " << evaluation << std::endl;
            }
            assert(evaluation == evaluation);
            if(evaluation > bestEvaluation) {
                bestEvaluation = evaluation;
                if(depth == maxdepth) bestAction = action;
            }
        }
            
        assert(bestEvaluation == bestEvaluation);
        return bestEvaluation;
    }


    double alphaBetaSearch(GameState currentState, int depth, double alpha, double beta) {
        if(depth == 0) {
            Agent::score score = agent.evaluate(currentState);
            double eval = score.value();
            assert(eval == eval);
            return eval;
        }
        ActionSet actionset = currentState.allowedActions();
        ActionOrdering orderer(actionset, currentState);
        orderer.sort();
        if(actionset.empty()) {
            if(currentState.hasWon(currentState.currentPlayer)) {
                return -agent.kingDeadValue + 0*depth;
            }
            if(currentState.hasLost(currentState.currentPlayer)) {
                return agent.kingDeadValue - 0*depth;
            }
            return 0;
        }

        double bestEvaluation = -std::numeric_limits<double>::infinity();
        for(Action action : actionset) {
            GameState tmp = currentState;
            tmp.apply(action);
            double evaluation = -discount*alphaBetaSearch(tmp, depth-1, -beta, -alpha);
            if(depth == maxdepth) {
                std::cout << action.toString() << " : " << evaluation << std::endl;
            }
            assert(evaluation == evaluation);
            if(evaluation > beta) {
                return beta;
            }
            if(evaluation > alpha) {
                alpha = evaluation;
                if(depth == maxdepth) bestAction = action;
            }
        }
            
        assert(alpha == alpha);
        return alpha;
    }

};


#endif