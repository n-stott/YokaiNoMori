#ifndef ENUMS_H
#define ENUMS_H

#include "constants.h"
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

enum Color : char {
    P1,
    P2,
    None,
};

enum ActionType : char {
    Move,
    Drop
};

enum BoardConfig : char {
    Easy,
    Medium   
};

#endif