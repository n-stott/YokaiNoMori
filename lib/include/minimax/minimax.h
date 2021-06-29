#ifndef MINIMAX_H
#define MINIMAX_H

#include "logger.h"
#include <limits>
#include <optional>


enum Mode {
    PureMinimax,
    AlphaBeta,
    IterativeDeepening
};

template<Mode mode, typename Action, typename ActionSet, typename GameState, typename Agent, typename ActionOrdering>
struct Minimax {

    GameState& root;
    std::optional<Action> bestAction;
    Agent& agent;


    Minimax(GameState& root, Agent& agent) :
        root(root),
        bestAction(),
        agent(agent)
    { }


    double run(int maxDepth) {
        const double inf = std::numeric_limits<double>::infinity();
        double res = 0.0;
        if(mode == PureMinimax) {
            res = search(root, maxDepth, maxDepth);
        }
        if(mode == AlphaBeta) {
            res = alphaBetaSearch(root, maxDepth, maxDepth, -inf, inf);
        }
        if(mode == IterativeDeepening) {
            res = iterativeDeepening(root, maxDepth);
        }
        if(res == -inf) {
            ActionSet actionset;
            root.fillAllowedActions(&actionset);
            bestAction = actionset[0];
        }
        return res;
    }

private:

    double search(GameState currentState, int maxDepth, int depth) {

        if(currentState.hasWon(currentState.currentPlayer)) {
            return std::numeric_limits<double>::infinity();
        }
        if(currentState.hasLost(currentState.currentPlayer)) {
            return -std::numeric_limits<double>::infinity();
        }
        if(currentState.hasDraw()) {
            return -std::numeric_limits<double>::infinity();
        }
        
        if(depth == 0) {
            typename Agent::score score = agent.evaluate(currentState);
            double eval = score.value(currentState.currentPlayer);
            assert(eval == eval);
            return eval;
        }

        ActionSet actionset;
        currentState.fillAllowedActions(&actionset);
        assert(!actionset.empty());

        {
            ActionOrdering orderer(&actionset, currentState);
            orderer.sort();
        }

        double bestEvaluation = -std::numeric_limits<double>::infinity();
        for(Action action : actionset) {
            GameState tmp = currentState;
            bool validMove = tmp.apply(action);
            assert(validMove);
            double evaluation = -search(tmp, maxDepth, depth-1);
            if(depth == maxDepth) {
                Logger::log(Verb::Dev, [&](){ return action.toString() + " : " + std::to_string(evaluation); });
            }
            assert(evaluation == evaluation);
            if(evaluation > bestEvaluation) {
                bestEvaluation = evaluation;
                if(depth == maxDepth) bestAction = action;
            }
            if(validMove) tmp.revert();
        }
            
        assert(bestEvaluation == bestEvaluation);
        return bestEvaluation;
    }


    double alphaBetaSearch(GameState currentState, int maxDepth, int depth, double alpha, double beta) {

        if(currentState.hasWon(currentState.currentPlayer)) {
            return std::numeric_limits<double>::infinity();
        }
        if(currentState.hasLost(currentState.currentPlayer)) {
            return -std::numeric_limits<double>::infinity();
        }
        if(currentState.hasDraw()) {
            return -std::numeric_limits<double>::infinity();
        }
        
        if(depth == 0) {
            typename Agent::score score = agent.evaluate(currentState);
            double eval = score.value(currentState.currentPlayer);
            assert(eval == eval);
            return eval;
        }

        ActionSet actionset;
        currentState.fillAllowedActions(&actionset);
        assert(!actionset.empty());

        {
            ActionOrdering orderer(&actionset, currentState);
            orderer.sort();
        }

        double bestEvaluation = -std::numeric_limits<double>::infinity();
        for(Action action : actionset) {
            GameState tmp = currentState;
#ifndef NDEBUG
            const size_t historySize1 = tmp.history->positions.size();
#endif
            bool validMove = tmp.apply(action);
            assert(validMove);
            double evaluation = -alphaBetaSearch(tmp, maxDepth, depth-1, -beta, -alpha);
            if(depth == maxDepth) {
                Logger::log(Verb::Dev, [&](){ return action.toString() + " : " + std::to_string(evaluation); });
            }
            assert(evaluation == evaluation);
            if(evaluation > beta) {
#ifndef NDEBUG
                assert(validMove);
                if(validMove)
#endif
                tmp.revert();
                return beta;
            }
            if(evaluation > alpha) {
                alpha = evaluation;
                if(depth == maxDepth) bestAction = action;
            }
#ifndef NDEBUG
            assert(validMove);
            if(validMove) 
#endif
            tmp.revert();
#ifndef NDEBUG
            const size_t historySize2 = tmp.history->positions.size();
            assert(historySize1 == historySize2);
#endif
        }
            
        assert(alpha == alpha);
        return alpha;
    }


    double alphaBetaSearchWithHint(GameState currentState, int maxDepth, int depth, double alpha, double beta, Action* hint) {

        if(currentState.hasWon(currentState.currentPlayer)) {
            return std::numeric_limits<double>::infinity();
        }
        if(currentState.hasLost(currentState.currentPlayer)) {
            return -std::numeric_limits<double>::infinity();
        }
        if(currentState.hasDraw()) {
            return -std::numeric_limits<double>::infinity();
        }
        
        if(depth == 0) {
            typename Agent::score score = agent.evaluate(currentState);
            double eval = score.value(currentState.currentPlayer);
            assert(eval == eval);
            return eval;
        }

        ActionSet actionset;
        currentState.fillAllowedActions(&actionset);
        assert(!actionset.empty());

        {
            ActionOrdering orderer(&actionset, currentState);
            orderer.sort();
        }

        auto tryAction = [&](Action action) {
            GameState tmp = currentState;
            const size_t historySize1 = tmp.history->positions.size();
            bool validMove = tmp.apply(action);
            assert(validMove);
            double evaluation = -alphaBetaSearch(tmp, maxDepth, depth-1, -beta, -alpha);
            if(depth == maxDepth) {
                Logger::log(Verb::Dev, [&](){ return action.toString() + " : " + std::to_string(evaluation); });
            }
            assert(evaluation == evaluation);
            if(evaluation > beta) {
                if(validMove) tmp.revert();
                return beta;
            }
            if(evaluation > alpha) {
                alpha = evaluation;
                if(depth == maxDepth) bestAction = action;
            }
            if(validMove) tmp.revert();
            const size_t historySize2 = tmp.history->positions.size();
            assert(historySize1 == historySize2);
            return std::numeric_limits<double>::quiet_NaN(); // return nan as "no return" flag
        };

        double bestEvaluation = -std::numeric_limits<double>::infinity();

        if(hint) {
            double val = tryAction(*hint);
            Logger::log(Verb::Dev, [&](){
                return R"(hint returns : )" 
                    + std::to_string(val) + R"( )"
                    + std::to_string(alpha) + R"( )"
                    + std::to_string(beta);
                });
            if(val == val) return val;
        }
        for(Action action : actionset) {
            double val = tryAction(action);
            if(val == val) return val;
        }
            
        assert(alpha == alpha);
        return alpha;
    }

    double iterativeDeepening(GameState currentState, int maxDepth) {
        const double inf = std::numeric_limits<double>::infinity();
        std::optional<Action> currentBest;
        bestAction.reset();
        double bestScore = -inf;
        for(int depth = 0; depth < maxDepth; ++depth) {
            Action hintAction;
            if(currentBest.has_value()) hintAction = currentBest.value();
            Action* hint = (currentBest.has_value() ? &hintAction : nullptr);
            Logger::log(Verb::Dev, [&](){
                return R"(Current hint is : )" + (hint ? hint->toString() : "none");    
            });
            bestScore = alphaBetaSearchWithHint(currentState, depth, depth, -inf, inf, hint);
            currentBest = bestAction;
        }
        return bestScore;
    }

};


#endif
