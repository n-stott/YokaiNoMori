#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "staticvector.h"
#include "enums.h"
#include "piece.h"
#include "gameconfig.h"

struct GameLogic {

    static constexpr unsigned int rows = GameConfig::rows;
    static constexpr unsigned int cols = GameConfig::cols;

    using move_set = typename AllowedMove::move_set;
    using move_sets = typename AllowedMove::move_sets;

    static constexpr const move_sets& moveSets(Piece piece) {
        assert(piece.id() < rows*cols);
        return allMoveSets[piece.id()];
    }

    static constexpr const move_set& moveSet(Piece piece, Pos position) {
        assert(piece.id() < allMoveSets.size());
        assert(position.idx() < allMoveSets[piece.id()].size());
        return allMoveSets[piece.id()][position.idx()];
    }

    static constexpr static_vector<move_sets, NB_PLAYERS*NB_PIECE_TYPE> allMoveSets {
        AllowedMove::computeMoveSets(PieceType::NoType , Color::P0),
        AllowedMove::computeMoveSets(PieceType::NoType , Color::P1),
        AllowedMove::computeMoveSets(PieceType::King   , Color::P0),
        AllowedMove::computeMoveSets(PieceType::King   , Color::P1),
        AllowedMove::computeMoveSets(PieceType::Rook   , Color::P0),
        AllowedMove::computeMoveSets(PieceType::Rook   , Color::P1),
        AllowedMove::computeMoveSets(PieceType::Bishop , Color::P0),
        AllowedMove::computeMoveSets(PieceType::Bishop , Color::P1),
        AllowedMove::computeMoveSets(PieceType::Pawn   , Color::P0),
        AllowedMove::computeMoveSets(PieceType::Pawn   , Color::P1),
        AllowedMove::computeMoveSets(PieceType::Queen  , Color::P0),
        AllowedMove::computeMoveSets(PieceType::Queen  , Color::P1)
    };

};

#endif