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


    int run(int maxDepth) {
        const int inf = std::numeric_limits<int>::max();
        int res = 0;
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

    int search(GameState currentState, int maxDepth, int depth) {

        if(currentState.hasWon(currentState.currentPlayer)) {
            return std::numeric_limits<int>::max();
        }
        if(currentState.hasLost(currentState.currentPlayer)) {
            return -std::numeric_limits<int>::max();
        }
        if(currentState.hasDraw()) {
            return agent.drawPenalty;
        }
        
        if(depth == 0) {
            typename Agent::score score = agent.evaluate(currentState);
            int eval = score.value(currentState.currentPlayer);
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

        int bestEvaluation = -std::numeric_limits<int>::max();
        for(Action action : actionset) {
            GameState tmp = currentState;
            bool validMove = tmp.apply(action);
            assert(validMove);
            int evaluation = -search(tmp, maxDepth, depth-1);
            if(depth == maxDepth) {
                Logger::with(Verb::Dev, [&](){ fmt::print("{} : {}\n", action.toString(), evaluation); });
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


    int alphaBetaSearch(GameState currentState, int maxDepth, int depth, int alpha, int beta) {

        if(currentState.hasWon(currentState.currentPlayer)) {
            return std::numeric_limits<int>::max();
        }
        if(currentState.hasLost(currentState.currentPlayer)) {
            return -std::numeric_limits<int>::max();
        }
        if(currentState.hasDraw()) {
            return -std::numeric_limits<int>::max();
        }
        
        if(depth == 0) {
            typename Agent::score score = agent.evaluate(currentState);
            int eval = score.value(currentState.currentPlayer);
            assert(eval == eval);
            return eval;
        }

        ActionSet actionset;
        currentState.fillAllowedActions(&actionset);
        if(actionset.empty()) {
            return -std::numeric_limits<int>::max();
        }
        assert(!actionset.empty());

        {
            ActionOrdering orderer(&actionset, currentState);
            orderer.sort();
        }

        int bestEvaluation = -std::numeric_limits<int>::max();
        for(Action action : actionset) {
            GameState tmp = currentState;
#ifndef NDEBUG
            const size_t historySize1 = tmp.history->positions.size();
#endif
            bool validMove = tmp.apply(action);
            assert(validMove);
            int evaluation = -alphaBetaSearch(tmp, maxDepth, depth-1, -beta, -alpha);
            if(depth == maxDepth) {
                Logger::with(Verb::Dev, [&](){ fmt::print("{} : {}\n", action.toString(), evaluation); });
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


    int alphaBetaSearchWithHint(GameState currentState, int maxDepth, int depth, int alpha, int beta, Action* hint) {

        if(currentState.hasWon(currentState.currentPlayer)) {
            return std::numeric_limits<int>::max();
        }
        if(currentState.hasLost(currentState.currentPlayer)) {
            return -std::numeric_limits<int>::max();
        }
        if(currentState.hasDraw()) {
            return -std::numeric_limits<int>::max();
        }
        
        if(depth == 0) {
            typename Agent::score score = agent.evaluate(currentState);
            int eval = score.value(currentState.currentPlayer);
            assert(eval == eval);
            return eval;
        }

        ActionSet actionset;
        currentState.fillAllowedActions(&actionset);
        if(actionset.empty()) {
            return -std::numeric_limits<int>::max();
        }

        {
            ActionOrdering orderer(&actionset, currentState);
            orderer.sort();
        }

        auto tryAction = [&](Action action) {
            GameState tmp = currentState;
            const size_t historySize1 = tmp.history->positions.size();
            bool validMove = tmp.apply(action);
            assert(validMove);
            int evaluation = -alphaBetaSearch(tmp, maxDepth, depth-1, -beta, -alpha);
            if(depth == maxDepth) {
                Logger::with(Verb::Dev, [&](){ fmt::print("{} : {}\n", action.toString(), evaluation); });
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
            return std::numeric_limits<int>::min(); // return nan as "no return" flag
        };

        int bestEvaluation = -std::numeric_limits<int>::max();

        if(hint) {
            int val = tryAction(*hint);
            Logger::with(Verb::Dev, [&](){
                fmt::print("hint returns : {} {} {}\n", val, alpha, beta);
            });
            if(val != std::numeric_limits<int>::min()) return val;
        }
        for(Action action : actionset) {
            int val = tryAction(action);
            if(val != std::numeric_limits<int>::min()) return val;
        }
            
        assert(alpha == alpha);
        return alpha;
    }

    int iterativeDeepening(GameState currentState, int maxDepth) {
        const int inf = std::numeric_limits<int>::max();
        std::optional<Action> currentBest;
        bestAction.reset();
        int bestScore = -inf;
        for(int depth = 0; depth < maxDepth; ++depth) {
            Action hintAction;
            if(currentBest.has_value()) hintAction = currentBest.value();
            Action* hint = (currentBest.has_value() ? &hintAction : nullptr);
            Logger::with(Verb::Dev, [&](){
                fmt::print("Current hint is : {}\n", hint ? hint->toString() : "none");
            });
            bestScore = alphaBetaSearchWithHint(currentState, depth, depth, -inf, inf, hint);
            currentBest = bestAction;
            if(bestScore == std::numeric_limits<int>::max()) break;
        }
        return bestScore;
    }

};


#endif
