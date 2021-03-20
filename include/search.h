#ifndef SEARCH_H
#define SEARCH_H

#include "gamestate.h"
#include "action.h"
#include "agent.h"
#include <limits>
#include <optional>
#include <iostream>

struct Search {

    int maxdepth;
    GameState& root;
    std::optional<Action> bestAction;
    double bestScore;
    Agent& agent;

    Search(GameState& root, Agent& agent, int maxdepth) :
        maxdepth(maxdepth),
        root(root),
        bestAction(),
        bestScore(-std::numeric_limits<double>::infinity()),
        agent(agent)
    { }

    double run() {
        return search(root, maxdepth);
    }

private:

    double search(GameState currentState, int depth) {
        if(depth == 0) {
            Agent::score score = agent.evaluate(currentState);
            double eval = score.value();
            assert(eval == eval);
            return eval;
        }
        ActionSet actionset = currentState.allowedActions();
        if(actionset.empty()) {
            if(currentState.hasWon(currentState.currentPlayer)) {
                return -agent.kingDeadValue - depth;
            }
            if(currentState.hasLost(currentState.currentPlayer)) {
                return agent.kingDeadValue + depth;
            }
            return 0;
        }

        double bestEvaluation = -std::numeric_limits<double>::infinity();
        for(Action action : actionset) {
            GameState tmp = currentState;
            tmp.apply(action);
            double evaluation = -search(tmp, depth-1);
            if(depth == maxdepth) {
                std::cout << action.toString() << " : " << evaluation << std::endl;
            }
            assert(evaluation == evaluation);
            if(evaluation >= bestEvaluation) {
                bestEvaluation = evaluation;
                if(depth == maxdepth) bestAction = action;
            }
        }
            
        assert(bestEvaluation == bestEvaluation);
        return bestEvaluation;
    }

};


#endif