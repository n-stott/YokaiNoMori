#ifndef AGENT_H
#define AGENT_H

#include "stateanalysis.h"
#include "enums.h"
#include "minimax/logger.h"
#include <algorithm>
#include <array>
#include <limits>
#include <cmath>

#if 0
#include <unordered_map>

struct state_hash {
    board_hash_t bh;
    reserve_hash_t r1h;
    reserve_hash_t r2h;

    bool operator==(const state_hash& other) const {
        return bh == other.bh && r1h == other.r1h && r2h == other.r2h;
    }
};

struct StateHasher {

    size_t operator()(const state_hash& sh) const {
        return sh.bh ^ sh.r1h ^ sh.r2h;
    }

};

template<BoardConfig config>
struct StateRecord {
    std::unordered_map<state_hash, StateAnalysis<config>, StateHasher> record;

    const StateAnalysis<config>& push(const GameState<config>& s) {
        state_hash sh {s.board.hash(), s.reserve1.hash(), s.reserve2.hash()};
        if(record.count(sh) == 0) {
            record[sh] = StateAnalysis<config>(s.board, s.reserve1, s.reserve2);
        }
        return record[sh];
    }

};
#endif

template<BoardConfig config>
struct Agent {

    struct score {
        double s1 = 0; // estimated score of p1
        double s2 = 0; // estimated score of p2
        double p = 0;  // penalty for p1 (non symetric)

        double value(Color player) const { return (player == Color::P1 ? (s1-s2) : (s2-s1))+p; }
    };

    size_t nbEvals;

    // scores
    std::array<double, NB_PIECE_TYPE> boardValue;
    std::array<double, NB_PIECE_TYPE> reserveValue;
    double occupiedValue;
    double controlledValue;
    double disputedValue;
    double dangerValue;
    double kingAttackedValue;
    double kingEscapesValue;
    double kingDistanceValue;
    double kingDeadValue;
    double endGamePenalty;
    double drawPenalty;

    Agent() : 
        nbEvals(0),
        boardValue{0, 0, 5, 3, 1, 4, 2},
        reserveValue{0, 1000, 10, 6, 2, 0, 4},
        occupiedValue(1),
        controlledValue(0.5),
        disputedValue(0),
        dangerValue(-0.5),
        kingAttackedValue(-5),
        kingEscapesValue(1),
        kingDistanceValue(1),
        kingDeadValue(-std::numeric_limits<double>::infinity()),
        endGamePenalty(-500),
        drawPenalty(-5000)
    { }

    ~Agent() {
        Logger::log(Verb::Dev, [&](){ return "Agent has evaluated : " + std::to_string(nbEvals) + " positions"; });
    }

    score evaluate(const GameState<config>& state) {
        ++nbEvals;

        StateAnalysis<config> sa(state.board, state.reserve1, state.reserve2);

        score s;
        s.s1 += occupiedValue * sa.nbOccupied1();
        s.s1 += controlledValue * sa.nbControlled1();
        s.s1 += disputedValue * sa.nbDisputed();
        s.s1 += dangerValue * sa.nbDanger1();
        s.s1 += kingAttackedValue * sa.isKingAttacked1();
        s.s1 += kingEscapesValue * sa.nbKingEscapes1();
        s.s1 += kingDistanceValue * sa.kingDistance1();
        s.s1 += (sa.hasKing1() ? 0 : kingDeadValue);
        for(size_t i = 0; i < NB_PIECE_TYPE; ++i) s.s1 += boardValue[i] * sa.onBoard1[i];
        for(size_t i = 0; i < NB_PIECE_TYPE; ++i) s.s1 += reserveValue[i] * sa.inReserve1[i];

        s.s2 += occupiedValue * sa.nbOccupied2();
        s.s2 += controlledValue * sa.nbControlled2();
        s.s2 += disputedValue * sa.nbDisputed();
        s.s2 += dangerValue * sa.nbDanger2();
        s.s2 += kingAttackedValue * sa.isKingAttacked2();
        s.s2 += kingEscapesValue * sa.nbKingEscapes2();
        s.s2 += kingDistanceValue * sa.kingDistance2();
        s.s2 += (sa.hasKing2() ? 0 : kingDeadValue);
        for(size_t i = 0; i < NB_PIECE_TYPE; ++i) s.s2 += boardValue[i] * sa.onBoard2[i];
        for(size_t i = 0; i < NB_PIECE_TYPE; ++i) s.s2 += reserveValue[i] * sa.inReserve2[i];

        // s.p += endGamePenalty * (1.0*state.nbTurns/state.maxTurns) * (1.0*state.nbTurns/state.maxTurns);
        s.p += drawPenalty * state.history->hasDraw();
        // s.p += 1.0 * state.board.hash() / std::numeric_limits<board_hash_t>::max();

        return s;
    }

};


#endif
