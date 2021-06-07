#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "staticvector.h"
#include "enums.h"
#include "piece.h"

template<unsigned int rows, unsigned int cols>
struct GameLogic {

    using move_set = typename AllowedMove<rows, cols>::move_set;
    using move_sets = typename AllowedMove<rows, cols>::move_sets;

    static inline const move_sets& moveSets(Piece piece) {
        assert(piece.data_ < 12);
        return allMoveSets[piece.data_];
    }

    static inline const move_set& moveSet(Piece piece, Pos position) {
        return allMoveSets[piece.data_][position.idx()];
    }

    static constexpr static_vector<move_sets, 12> allMoveSets {
        AllowedMove<rows, cols>::computeMoveSets(PieceType::NoType   , Color::P1),
        AllowedMove<rows, cols>::computeMoveSets(PieceType::NoType   , Color::P2),
        AllowedMove<rows, cols>::computeMoveSets(PieceType::King     , Color::P1),
        AllowedMove<rows, cols>::computeMoveSets(PieceType::King     , Color::P2),
        AllowedMove<rows, cols>::computeMoveSets(PieceType::Tower    , Color::P1),
        AllowedMove<rows, cols>::computeMoveSets(PieceType::Tower    , Color::P2),
        AllowedMove<rows, cols>::computeMoveSets(PieceType::Bishop   , Color::P1),
        AllowedMove<rows, cols>::computeMoveSets(PieceType::Bishop   , Color::P2),
        AllowedMove<rows, cols>::computeMoveSets(PieceType::Pawn     , Color::P1),
        AllowedMove<rows, cols>::computeMoveSets(PieceType::Pawn     , Color::P2),
        AllowedMove<rows, cols>::computeMoveSets(PieceType::SuperPawn, Color::P1),
        AllowedMove<rows, cols>::computeMoveSets(PieceType::SuperPawn, Color::P2)
    };

};

#endif