#ifndef ENUMS_H
#define ENUMS_H

enum PieceType {
    NoType,
    King,
    Tower,
    Rook,
    Pawn,
    SuperPawn
};

enum Color {
    None,
    P1,
    P2
};

enum ActionType {
    Move,
    Drop
};


#endif