#ifndef ENUMS_H
#define ENUMS_H

#include "constants.h"
#include <cstddef>

enum PieceType : char {
    NoType,
    King,
    Rook,
    Bishop,
    Pawn,
    Queen
};

enum Color : char {
    P0,
    P1,
    None,
};

enum ActionType : char {
    Move,
    Drop
};

#endif