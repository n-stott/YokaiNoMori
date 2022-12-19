#ifndef AGENT_H
#define AGENT_H

#include "stateanalysis.h"
#include "enums.h"
#include "minimax/logger.h"
#include <algorithm>
#include <array>
#include <limits>
#include <cmath>

struct Agent {

    struct score {
        double s0 = 0; // estimated score of p0
        double s1 = 0; // estimated score of p1
        double p = 0;  // penalty

        double value(Color player) const { return (player == Color::P0 ? (s0-s1) : (s1-s0))+p; }
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
        boardValue{0, 0, 5, 3, 1, 4},
        reserveValue{0, 1000, 10, 6, 2, 0},
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
        Logger::with(Verb::Dev, [&](){ fmt::print("Agent has evaluated {} positions\n", nbEvals); });
    }

    score evaluate(const GameState& state) {
        ++nbEvals;

        StateAnalysis sa(state.board, state.reserve0, state.reserve1);

        score s;
        s.s0 += occupiedValue * sa.nbOccupied0();
        s.s0 += controlledValue * sa.nbControlled0();
        s.s0 += disputedValue * sa.nbDisputed();
        s.s0 += dangerValue * sa.nbDanger0();
        s.s0 += kingAttackedValue * sa.isKingAttacked0();
        s.s0 += kingEscapesValue * sa.nbKingEscapes0();
        s.s0 += kingDistanceValue * sa.kingDistance0();
        s.s0 += (sa.hasKing0() ? 0 : kingDeadValue);
        for(size_t i = 0; i < NB_PIECE_TYPE; ++i) s.s0 += boardValue[i] * sa.onBoard0[i];
        for(size_t i = 0; i < NB_PIECE_TYPE; ++i) s.s0 += reserveValue[i] * sa.inReserve0[i];

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

        s.p += drawPenalty * (state.history ? state.history->hasDraw() : 0);

        return s;
    }

};


#endif
