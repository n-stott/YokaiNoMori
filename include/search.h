#ifndef SEARCH_H
#define SEARCH_H

#include "state.h"
#include "action.h"
#include "agent.h"
#include <limits>

struct Search {

    State& root;
    Action bestAction;
    Action tmpBestAction;

    Agent& agent;

    Search(State& root, Agent& agent) :
        root(root),
        bestAction(),
        tmpBestAction(),
        agent(agent)
    { }

    double search(int depth) {
        return search(root, depth, agent.player);
    }

private:

    double search(State currentState, int depth, Color playerTurn) {
        if(depth == 0) {
            Agent::score score = agent.evaluate(currentState);
            double eval = score.value(playerTurn);
            assert(eval == eval);
            return eval;
        }
        ActionSet actionset = currentState.allowedActions();
        if(actionset.empty()) {
            if(currentState.hasWon(playerTurn)) {
                return std::numeric_limits<double>::infinity();
            }
            if(currentState.hasLost(playerTurn)) {
                return -std::numeric_limits<double>::infinity();
            }
            return 0;
        }

        double bestEvaluation = -std::numeric_limits<double>::infinity();
        for(Action action : actionset) {
            State tmp = currentState;
            tmp.apply(action);
            double evaluation = search(tmp, depth-1, (playerTurn == P1 ? P2 : P1));
            assert(evaluation == evaluation);
            bestEvaluation = std::max(evaluation, bestEvaluation);
        }
            
        assert(bestEvaluation == bestEvaluation);
        return bestEvaluation;
    }

};


#endif