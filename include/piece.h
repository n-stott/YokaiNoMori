#ifndef PIECE_H
#define PIECE_H

#include "staticvector.h"
#include "pos.h"
#include "enums.h"

struct Offset {
    static constexpr static_vector<Pos::type, 8> empty { };
    static constexpr static_vector<Pos::type, 8> king { Pos::v, Pos::v+Pos::h, Pos::h, Pos::h-Pos::v, -Pos::v, -Pos::v-Pos::h, -Pos::h, -Pos::h+Pos::v };
    static constexpr static_vector<Pos::type, 8> tower { Pos::v, Pos::h, -Pos::v, -Pos::h };
    static constexpr static_vector<Pos::type, 8> rook { +Pos::v+Pos::h, +Pos::v-Pos::h, -Pos::v+Pos::h, -Pos::v-Pos::h };
    static constexpr static_vector<Pos::type, 8> p1Pawn { Pos::v };
    static constexpr static_vector<Pos::type, 8> p2Pawn { -Pos::v };
    static constexpr static_vector<Pos::type, 8> p1SuperPawn { Pos::v, Pos::v+Pos::h, Pos::h, -Pos::v, -Pos::h, -Pos::h+Pos::v };
    static constexpr static_vector<Pos::type, 8> p2SuperPawn { Pos::v, Pos::h, Pos::h-Pos::v, -Pos::v, -Pos::v-Pos::h, -Pos::h };
};

class Piece {
private:
    uint8_t data_;

public:
    constexpr Piece() noexcept : data_(0) {}
    constexpr Piece(PieceType pt, Color c) noexcept : data_( pt | (c << 3) ) {}

    constexpr bool empty() const noexcept { return data_ == 0; }
    
    constexpr PieceType type() const noexcept { return (PieceType)(data_ & 0b0000111); }
    constexpr Color color() const noexcept    { return (Color)(data_ >> 3); }
    constexpr void setColor(Color c) noexcept { data_ = (data_ & 0b00000111) | (c << 3); }

    constexpr char toChar() const noexcept {
        char ret = '.';
        if(type() == King) ret = 'k';
        if(type() == Tower) ret = 't';
        if(type() == Rook) ret = 'r';
        if(type() == Pawn) ret = 'p';
        if(type() == SuperPawn) ret = 's';
        if(color() == P2) ret -= 32;
        return ret;
    }

    const static_vector<Pos::type, 8>& offsets() const {
        if(type() == King) return Offset::king;
        if(type() == Tower) return Offset::tower;
        if(type() == Rook) return Offset::rook;
        if(color() == P1 && type() == Pawn) return Offset::p1Pawn;
        if(color() == P1 && type() == SuperPawn) return Offset::p1SuperPawn;
        if(color() == P2 && type() == Pawn) return Offset::p2Pawn;
        if(color() == P2 && type() == SuperPawn) return Offset::p2SuperPawn;
        return Offset::empty;
    }

private:

};

#endif