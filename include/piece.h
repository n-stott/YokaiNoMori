#ifndef PIECE_H
#define PIECE_H

#include "allowedmove.h"
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

    const AllowedMove::move_sets& moveSets() const {
        if(type() == King) return AllowedMove::king;
        if(type() == Tower) return AllowedMove::tower;
        if(type() == Rook) return AllowedMove::rook;
        if(color() == P1 && type() == Pawn) return AllowedMove::p1Pawn;
        if(color() == P1 && type() == SuperPawn) return AllowedMove::p1SuperPawn;
        if(color() == P2 && type() == Pawn) return AllowedMove::p2Pawn;
        if(color() == P2 && type() == SuperPawn) return AllowedMove::p2SuperPawn;
        return AllowedMove::empty;
    }

    const AllowedMove::move_set& moveSet(Pos position) const {
        return moveSets()[position.idx()];
    }

private:

};

#endif