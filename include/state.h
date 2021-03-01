#ifndef STATE_H
#define STATE_H

#include <array>

enum Color : short {
    None,
    P1,
    P2
};

static constexpr short COLORS = 3;

enum Type : short {
    Pawn = 0x0,
    Rook = 0x1,
    Tower = 0x2,
    King = 0x3,
    SuperPawn = 0x10,
};

static constexpr short BASE_MASK = 0xF;

static constexpr short BASE_TYPES = 4;

struct Piece {
    Color color;
    Type type; 
};

struct Reserve {
    static constexpr int MaxPiece = 8;
    std::array<Piece, MaxPiece> reserve;
    short size;
    const Color color;

    const Piece* begin() const { return reserve.data(); }
    const Piece* end() const { return reserve.data() + size; }

    void insert(const Piece piece) {
        reserve[size] = piece;
        reserve[size].color = color;
        ++size;
    }

    void remove(const short position) {
        --size;
        if(position != size) {
            std::swap(reserve[position], reserve[size]);
        }
    }
};

struct Board {
    static constexpr int R = 4;
    static constexpr int C = 3;

    std::array<Piece, R*C> board;

    const Piece* begin() const { return board.begin(); }
    const Piece* end() const { return board.end(); }
};

struct State {
    Board board;
    Reserve r1;
    Reserve r2;
};

struct Move {
    short start;
    short end;
};


#endif