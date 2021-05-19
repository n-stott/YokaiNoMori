#ifndef MINIMAX_H
#define MINIMAX_H

#include "logger.h"
#include <limits>
#include <optional>


enum Mode {
    PureMinimax,
    AlphaBeta
};

template<Mode mode, typename Action, typename GameState, typename Agent, typename ActionOrdering>
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


    double run() {
        const double inf = std::numeric_limits<double>::infinity();
        double res = 0.0;
        if(mode == PureMinimax) {
            res = search(root, maxdepth);
        }
        if(mode == AlphaBeta) {
            res = alphaBetaSearch(root, maxdepth, -inf, inf);
        }
        if(res == -inf) {
            ActionSet actionset;
            root.fillAllowedActions(&actionset);
            bestAction = actionset[0];
        }
        return res;
    }

private:

    static constexpr double applyBias(double v) {
        return v;
    }

    double search(GameState currentState, int depth) {
        {
            double eval = 1337;
            if(depth == 0) {
                typename Agent::score score = agent.evaluate(currentState);
                eval = score.value(currentState.currentPlayer);
            }
            if(depth == 0) {
                assert(eval == eval);
                return eval;
            }
        }
        ActionSet actionset;
        currentState.fillAllowedActions(&actionset);
        if(actionset.empty()) {
            if(currentState.hasWon(currentState.currentPlayer)) {
                return +std::numeric_limits<double>::infinity();
            } else if(currentState.hasLost(currentState.currentPlayer)) {
                return -std::numeric_limits<double>::infinity();
            }
            if(currentState.nbTurns == currentState.maxTurns) return 0;
            assert(false);
            return 0;
        }

        {
            ActionOrdering orderer(&actionset, currentState);
            orderer.sort();
        }

        double bestEvaluation = -std::numeric_limits<double>::infinity();
        for(Action action : actionset) {
            GameState tmp = currentState;
            tmp.apply(action);
            double evaluation = applyBias(-search(tmp, depth-1));
            if(depth == maxdepth) {
                Logger::log(Verb::Dev, [&](){ return action.toString() + " : " + std::to_string(evaluation); });
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
            typename Agent::score score = agent.evaluate(currentState);
            double eval = score.value(currentState.currentPlayer);
            assert(eval == eval);
            return eval;
        }
        ActionSet actionset;
        currentState.fillAllowedActions(&actionset);

        {
            ActionOrdering orderer(&actionset, currentState);
            orderer.sort();
        }
        
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
            double evaluation = applyBias(-alphaBetaSearch(tmp, depth-1, -beta, -alpha));
            if(depth == maxdepth) {
                Logger::log(Verb::Dev, [&](){ return action.toString() + " : " + std::to_string(evaluation); });
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
