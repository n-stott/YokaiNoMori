#ifndef ENUMS_H
#define ENUMS_H

#include <cstddef>

enum PieceType : char {
    NoType,
    King,
    Tower,
    Bishop,
    Pawn,
    SuperPawn,
    ArchBishop,
};

static constexpr unsigned int NB_PIECE_TYPE = 7;

enum Color : char {
    P1,
    P2,
    None,
};

static constexpr unsigned int NB_PLAYERS = 2;

enum ActionType : char {
    Move,
    Drop
};

enum BoardConfig : char {
    Easy,
    Medium   
};

#endif