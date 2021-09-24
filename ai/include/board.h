#ifndef BOARD_H
#define BOARD_H

#include "pos.h"
#include "piece.h"
#include "enums.h"
#include <algorithm>
#include <cstring>
#include <cassert>

struct Board {

    static constexpr int rows = 4;
    static constexpr int cols = 3;

    Board() :
        k0(1),
        k1(10),
        pieces({
            // it's backwards in the initializer...
            Piece(Bishop, P0), Piece(King, P0), Piece(Rook, P0),
            Piece(),           Piece(Pawn, P0), Piece(),
            Piece(),           Piece(Pawn, P1), Piece(),
            Piece(Rook, P1),   Piece(King, P1), Piece(Bishop, P1),
        })
    { }

    Board(const char* board) {
        assert(std::strlen(board) == rows*cols);
        for(size_t i = 0; i < rows*cols; ++i) {
            pieces[i] = Piece(board[i]);
        }
    }

    const Piece* begin() const noexcept { return pieces.begin(); }
    const Piece* end() const noexcept { return pieces.end(); }

    Piece get(uint8_t i) const {
        check();
        return pieces[i];
    }

    std::string toString() const {
        std::string s;
        for(Piece p : *this) s+=p.toChar();
        return s;
    }
    
    void set(uint8_t i, Piece p) {
        if(pieces[i].type() == PieceType::King) {
            if(pieces[i].color() == Color::P0) k0 = Pos(-1);
            if(pieces[i].color() == Color::P1) k1 = Pos(-1);
        }
        pieces[i] = p;
        if(p.type() == PieceType::King) {
            if(p.color() == Color::P0) k0 = Pos(i);
            if(p.color() == Color::P1) k1 = Pos(i);
        }
    }

    bool operator==(const Board& other) const {
        return ((k0 == other.k0) & (k1 == other.k1)) && pieces == other.pieces;
    }

    constexpr uint8_t king0() const { return k0.idx() < rows*cols ? k0.idx() : 0; }
    constexpr uint8_t king1() const { return k1.idx() < rows*cols ? k1.idx() : 0; }

private:

    void check() const {
        if(k0.idx() < rows*cols) assert(pieces[k0.idx()].type() == PieceType::King);
        if(k1.idx() < rows*cols) assert(pieces[k1.idx()].type() == PieceType::King);
    }

    Pos k0;
    Pos k1;
    std::array<Piece, rows*cols> pieces;

};

#endif