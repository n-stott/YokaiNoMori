#ifndef PIECE_H
#define PIECE_H

#include "staticvector.h"
#include "pos.h"
#include "enums.h"

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
        if(type() == King) return kingOffsets;
        if(type() == Tower) return towerOffsets;
        if(type() == Rook) return rookOffsets;
        if(color() == P1 && type() == Pawn) return p1PawnOffsets;
        if(color() == P1 && type() == SuperPawn) return p1SuperPawnOffsets;
        if(color() == P2 && type() == Pawn) return p2PawnOffsets;
        if(color() == P2 && type() == SuperPawn) return p2SuperPawnOffsets;
        assert(false);
        return kingOffsets;
    }

private:

    static static_vector<Pos::type, 8> kingOffsets;
    static static_vector<Pos::type, 8> towerOffsets;
    static static_vector<Pos::type, 8> rookOffsets;
    static static_vector<Pos::type, 8> p1PawnOffsets;
    static static_vector<Pos::type, 8> p2PawnOffsets;
    static static_vector<Pos::type, 8> p1SuperPawnOffsets;
    static static_vector<Pos::type, 8> p2SuperPawnOffsets;
};

static_vector<Pos::type, 8> Piece::kingOffsets {
    Pos::v, Pos::v+Pos::h, Pos::h, Pos::h-Pos::v, -Pos::v, -Pos::v-Pos::h, -Pos::h, -Pos::h+Pos::v
};

static_vector<Pos::type, 8> Piece::towerOffsets {
    Pos::v, Pos::h, -Pos::v, -Pos::h
};

static_vector<Pos::type, 8> Piece::rookOffsets {
    +Pos::v+Pos::h, +Pos::v-Pos::h, -Pos::v+Pos::h, -Pos::v-Pos::h
};

static_vector<Pos::type, 8> Piece::p1PawnOffsets {
    Pos::v
};

static_vector<Pos::type, 8> Piece::p2PawnOffsets {
    -Pos::v
};

static_vector<Pos::type, 8> Piece::p1SuperPawnOffsets {
    Pos::v, Pos::v+Pos::h, Pos::h, -Pos::v, -Pos::h, -Pos::h+Pos::v
};

static_vector<Pos::type, 8> Piece::p2SuperPawnOffsets {
    Pos::v, Pos::h, Pos::h-Pos::v, -Pos::v, -Pos::v-Pos::h, -Pos::h
};

#endif