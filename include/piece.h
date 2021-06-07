#ifndef PIECE_H
#define PIECE_H

#include "allowedmove.h"
#include "staticvector.h"
#include "pos.h"
#include "enums.h"


class Piece {
private:
    uint8_t data_;

    static constexpr uint8_t Empty = 0;

    static_assert(P1 == 0);
    static_assert(P2 == 1);

public:
    constexpr Piece() noexcept : data_(Empty) {}
    constexpr Piece(PieceType pt, Color c) noexcept : data_( c | (pt << 1) ) {}

    constexpr explicit Piece(int data) : data_(data) { }

    constexpr explicit Piece(char asChar) noexcept : data_(Empty) {
        switch(asChar) {
            case '.': data_ = 0; return;
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

    constexpr inline bool empty() const noexcept { return data_ == Empty; }
    
    constexpr inline PieceType type() const noexcept { return (PieceType)(data_ >> 1); }
    constexpr inline Color color() const noexcept    { return (Color)(data_ & 1); }

    constexpr inline void setType(PieceType pt) noexcept { data_ = (data_ & 1) | (pt << 1); }
    constexpr inline void setColor(Color c) noexcept {
        assert(c != None);
        data_ = (data_ & ~1) | c;
    }

    constexpr inline char toChar() const noexcept {
        return charCodes[data_];
    }

    constexpr inline void promote() noexcept {
        if(type() == Pawn) setType(SuperPawn);
    }

    constexpr inline void demote() noexcept {
        if(type() == SuperPawn) setType(Pawn);
    }

    inline const AllowedMove::move_sets& moveSets() const {
        assert(data_ < 12);
        return allMoveSets[data_];
    }

    inline const AllowedMove::move_set& moveSet(Pos position) const {
        return moveSets()[position.idx()];
    }

private:

    static constexpr static_vector<AllowedMove::move_sets, 12> allMoveSets {
        AllowedMove::computeMoveSets(PieceType::NoType   , Color::P1),
        AllowedMove::computeMoveSets(PieceType::NoType   , Color::P2),
        AllowedMove::computeMoveSets(PieceType::King     , Color::P1),
        AllowedMove::computeMoveSets(PieceType::King     , Color::P2),
        AllowedMove::computeMoveSets(PieceType::Tower    , Color::P1),
        AllowedMove::computeMoveSets(PieceType::Tower    , Color::P2),
        AllowedMove::computeMoveSets(PieceType::Bishop   , Color::P1),
        AllowedMove::computeMoveSets(PieceType::Bishop   , Color::P2),
        AllowedMove::computeMoveSets(PieceType::Pawn     , Color::P1),
        AllowedMove::computeMoveSets(PieceType::Pawn     , Color::P2),
        AllowedMove::computeMoveSets(PieceType::SuperPawn, Color::P1),
        AllowedMove::computeMoveSets(PieceType::SuperPawn, Color::P2)
    };

    static constexpr static_vector<char, 12> charCodes {
        '.',
        '.',
        'k',
        'K',
        't',
        'T',
        'b',
        'B',
        'p',
        'P',
        's',
        'S'
    };

};

static_assert(Piece( 0).type() == NoType    && Piece(0).color() == P1);
static_assert(Piece( 1).type() == NoType    && Piece(1).color() == P2);
static_assert(Piece( 2).type() == King      && Piece(2).color() == P1);
static_assert(Piece( 3).type() == King      && Piece(3).color() == P2);
static_assert(Piece( 4).type() == Tower     && Piece(4).color() == P1);
static_assert(Piece( 5).type() == Tower     && Piece(5).color() == P2);
static_assert(Piece( 6).type() == Bishop    && Piece(6).color() == P1);
static_assert(Piece( 7).type() == Bishop    && Piece(7).color() == P2);
static_assert(Piece( 8).type() == Pawn      && Piece(8).color() == P1);
static_assert(Piece( 9).type() == Pawn      && Piece(9).color() == P2);
static_assert(Piece(10).type() == SuperPawn && Piece(10).color() == P1);
static_assert(Piece(11).type() == SuperPawn && Piece(11).color() == P2);

static_assert(sizeof(Piece) == 1);

#endif
