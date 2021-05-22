#ifndef PIECE_H
#define PIECE_H

#include "allowedmove.h"
#include "staticvector.h"
#include "pos.h"
#include "enums.h"


class Piece {
private:
    uint8_t data_;

    static constexpr uint8_t Empty = -1;

    static_assert(P1 == 0);
    static_assert(P2 == 1);

public:
    constexpr Piece() noexcept : data_(Empty) {}
    constexpr Piece(PieceType pt, Color c) noexcept : data_( c | (pt << 1) ) {}

    constexpr explicit Piece(char asChar) noexcept : data_(Empty) {
        switch(asChar) {
            case 'e': data_ = Empty; return;
            case 'E': data_ = Empty; return;
            case 'k': data_ = P1 | (King << 1); return;
            case 't': data_ = P1 | (Tower << 1); return;
            case 'b': data_ = P1 | (Bishop << 1); return;
            case 'p': data_ = P1 | (Pawn << 1); return;
            case 's': data_ = P1 | (SuperPawn << 1); return;
            case 'K': data_ = P2 | (King << 1); return;
            case 'T': data_ = P2 | (Tower << 1); return;
            case 'B': data_ = P2 | (Bishop << 1); return;
            case 'P': data_ = P2 | (Pawn << 1); return;
            case 'S': data_ = P2 | (SuperPawn << 1); return;
        }
        assert(false);
    }

    constexpr bool empty() const noexcept { return data_ == Empty; }
    
    constexpr PieceType type() const noexcept { return (PieceType)(data_ >> 1); }
    constexpr Color color() const noexcept    { return (Color)(data_ & 1); }

    constexpr void setType(PieceType pt) noexcept { data_ = (data_ & 1) | (pt << 1); }
    constexpr void setColor(Color c) noexcept {
        assert(c != None);
        data_ = (data_ & ~1) | c;
    }

    constexpr char toChar() const noexcept {
        char ret = '.';
        if(type() == King) ret = 'k';
        if(type() == Tower) ret = 't';
        if(type() == Bishop) ret = 'b';
        if(type() == Pawn) ret = 'p';
        if(type() == SuperPawn) ret = 's';
        if(color() == P2) ret -= 32;
        if(empty()) ret = '.';
        return ret;
    }

    constexpr void promote() noexcept {
        if(type() == Pawn) setType(SuperPawn);
    }

    constexpr void demote() noexcept {
        if(type() == SuperPawn) setType(Pawn);
    }

    const AllowedMove::move_sets& moveSets() const {
        if(type() == King) return AllowedMove::king;
        if(type() == Tower) return AllowedMove::tower;
        if(type() == Bishop) return AllowedMove::bishop;
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

static_assert(sizeof(Piece) == 1);

#endif