#ifndef ENUMS_H
#define ENUMS_H

#include <cstddef>

enum PieceType : char {
    NoType,
    King,
    Tower,
    Bishop,
    Pawn,
    SuperPawn
};

enum Color : char {
    P1,
    P2,
    None,
};

enum ActionType : char {
    Move,
    Drop
};


#endif