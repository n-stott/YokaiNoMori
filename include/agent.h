#ifndef AGENT_H
#define AGENT_H

#include <array>
#include "boardsnapshot.h"
#include "enums.h"
#include <algorithm>

struct Agent {

    Color player;

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

    Agent() : 
        boardValue{0, 0, 5, 3, 1, 4},
        reserveValue{0, 0, 5, 3, 1, 0},
        occupiedValue(1),
        controlledValue(1),
        disputedValue(0),
        dangerValue(-1),
        kingAttackedValue(1),
        kingEscapesValue(1),
        kingDistanceValue(1)
    { }

    double evaluate(const State& s) const {
        StateAnalysis sa(s);
        double score1 = 0;
        score1 += occupiedValue * sa.nbOccupied1();
        score1 += controlledValue * sa.nbControlled1();
        score1 += disputedValue * sa.nbDisputed();
        score1 += dangerValue * sa.nbDanger1();
        score1 += kingAttackedValue * sa.isKingAttacked1();
        score1 += kingEscapesValue * sa.nbKingEscapes1();
        score1 += kingDistanceValue * sa.kingDistance1();
        for(size_t i = 0; i < 6; ++i) score1 += boardValue[i] * sa.onBoard1[i];
        for(size_t i = 0; i < 6; ++i) score1 += reserveValue[i] * sa.inReserve1[i];
        
        double score2 = 0;
        score2 += occupiedValue * sa.nbOccupied2();
        score2 += controlledValue * sa.nbControlled2();
        score2 += disputedValue * sa.nbDisputed();
        score2 += dangerValue * sa.nbDanger2();
        score2 += kingAttackedValue * sa.isKingAttacked2();
        score2 += kingEscapesValue * sa.nbKingEscapes2();
        score2 += kingDistanceValue * sa.kingDistance2();
        for(size_t i = 0; i < 6; ++i) score2 += boardValue[i] * sa.onBoard2[i];
        for(size_t i = 0; i < 6; ++i) score2 += reserveValue[i] * sa.inReserve2[i];

        return (player == Color::P1 ? score1-score2 : score2-score1);
    }

};


#endif