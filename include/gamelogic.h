#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "staticvector.h"
#include "enums.h"
#include "piece.h"
#include "gameconfig.h"

template<BoardConfig config>
struct GameLogic {

    static constexpr unsigned int rows = GameConfig<config>::rows;
    static constexpr unsigned int cols = GameConfig<config>::cols;

    using move_set = typename AllowedMove<config>::move_set;
    using move_sets = typename AllowedMove<config>::move_sets;

    static inline const move_sets& moveSets(Piece piece) {
        assert(piece.id() < rows*cols);
        return allMoveSets[piece.id()];
    }

    static inline const move_set& moveSet(Piece piece, Pos position) {
        assert(piece.id() < allMoveSets.size());
        assert(position.idx() < allMoveSets[piece.id()].size());
        return allMoveSets[piece.id()][position.idx()];
    }

    static constexpr static_vector<move_sets, 14> allMoveSets {
        AllowedMove<config>::computeMoveSets(PieceType::NoType    , Color::P1),
        AllowedMove<config>::computeMoveSets(PieceType::NoType    , Color::P2),
        AllowedMove<config>::computeMoveSets(PieceType::King      , Color::P1),
        AllowedMove<config>::computeMoveSets(PieceType::King      , Color::P2),
        AllowedMove<config>::computeMoveSets(PieceType::Tower     , Color::P1),
        AllowedMove<config>::computeMoveSets(PieceType::Tower     , Color::P2),
        AllowedMove<config>::computeMoveSets(PieceType::Bishop    , Color::P1),
        AllowedMove<config>::computeMoveSets(PieceType::Bishop    , Color::P2),
        AllowedMove<config>::computeMoveSets(PieceType::Pawn      , Color::P1),
        AllowedMove<config>::computeMoveSets(PieceType::Pawn      , Color::P2),
        AllowedMove<config>::computeMoveSets(PieceType::SuperPawn , Color::P1),
        AllowedMove<config>::computeMoveSets(PieceType::SuperPawn , Color::P2),
        AllowedMove<config>::computeMoveSets(PieceType::ArchBishop, Color::P1),
        AllowedMove<config>::computeMoveSets(PieceType::ArchBishop, Color::P2)
    };

};

#endif