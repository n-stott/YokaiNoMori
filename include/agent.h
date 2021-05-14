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
        double s1 = 0; // estimated score of p1
        double s2 = 0; // estimated score of p2
        double p = 0;  // penalty for p1 (non symetric)

        double value() const { return s1-s2+p; }
    };

    size_t nbEvals;

    // scores
    std::array<double, 6> boardValue;
    std::array<double, 6> reserveValue;
    double occupiedValue;
    double controlledValue;
    double disputedValue;
    double dangerValue;
    double kingAttackedValue;
    double kingEscapesValue;
    double kingDistanceValue;
    double kingDeadValue;
    double endGamePenalty;

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
        kingDeadValue(-10000),
        endGamePenalty(-50)
    { }

    ~Agent() {
        Logger::log(Verb::Dev, [&](){ return "Agent has evaluated : " + std::to_string(nbEvals) + " positions"; });
    }

    score evaluate(const GameState& state) {
        ++nbEvals;
        StateAnalysis sa(state);
        score s;
        s.s1 += occupiedValue * sa.nbOccupied1();
        s.s1 += controlledValue * sa.nbControlled1();
        s.s1 += disputedValue * sa.nbDisputed();
        s.s1 += dangerValue * sa.nbDanger1();
        s.s1 += kingAttackedValue * sa.isKingAttacked1();
        s.s1 += kingEscapesValue * sa.nbKingEscapes1();
        s.s1 += kingDistanceValue * sa.kingDistance1();
        s.s1 += (sa.hasKing1() ? 0 : kingDeadValue);
        for(size_t i = 0; i < 6; ++i) s.s1 += boardValue[i] * sa.onBoard1[i];
        for(size_t i = 0; i < 6; ++i) s.s1 += reserveValue[i] * sa.inReserve1[i];

        s.s2 += occupiedValue * sa.nbOccupied2();
        s.s2 += controlledValue * sa.nbControlled2();
        s.s2 += disputedValue * sa.nbDisputed();
        s.s2 += dangerValue * sa.nbDanger2();
        s.s2 += kingAttackedValue * sa.isKingAttacked2();
        s.s2 += kingEscapesValue * sa.nbKingEscapes2();
        s.s2 += kingDistanceValue * sa.kingDistance2();
        s.s2 += (sa.hasKing2() ? 0 : kingDeadValue);
        for(size_t i = 0; i < 6; ++i) s.s2 += boardValue[i] * sa.onBoard2[i];
        for(size_t i = 0; i < 6; ++i) s.s2 += reserveValue[i] * sa.inReserve2[i];

        s.p = endGamePenalty * (1.0*state.nbTurns/state.maxTurns) * (1.0*state.nbTurns/state.maxTurns);

        return s;
    }

};


#endif