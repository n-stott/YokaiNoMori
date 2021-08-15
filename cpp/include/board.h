#ifndef BOARD_H
#define BOARD_H

#include "pos.h"
#include "piece.h"
#include "enums.h"
#include <algorithm>
#include <cstring>
#include <cassert>

template<unsigned int rows, unsigned int cols>
struct Board {

    template<unsigned int R = rows, unsigned int C = cols>
    Board(typename std::enable_if<R==4 && C==3>::type* = 0) :
        k1(10),
        k2(1),
        pieces({
            Piece(Bishop, P2), Piece(King, P2), Piece(Tower, P2),
            Piece(),           Piece(Pawn, P2), Piece(),
            Piece(),           Piece(Pawn, P1), Piece(),
            Piece(Tower, P1),  Piece(King, P1), Piece(Bishop, P1)
        })
    { }


    template<unsigned int R = rows, unsigned int C = cols>
    Board(typename std::enable_if<R==6 && C==5>::type* = 0) :
        k1(27),
        k2(2),
        pieces({
            Piece(ArchBishop, P2), Piece(SuperPawn, P2), Piece(King, P2), Piece(SuperPawn, P2), Piece(ArchBishop, P2), 
            Piece(),               Piece(),              Piece(),         Piece(),              Piece(),
            Piece(),               Piece(Pawn, P2),      Piece(Pawn, P2), Piece(Pawn, P2),      Piece(),
            Piece(),               Piece(Pawn, P1),      Piece(Pawn, P1), Piece(Pawn, P1),      Piece(),
            Piece(),               Piece(),              Piece(),         Piece(),              Piece(),
            Piece(ArchBishop, P1), Piece(SuperPawn, P1), Piece(King, P1), Piece(SuperPawn, P1), Piece(ArchBishop, P1)
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
    
    void  set(uint8_t i, Piece p) {
        if(pieces[i].type() == PieceType::King) {
            if(pieces[i].color() == Color::P1) k1 = Pos(-1);
            if(pieces[i].color() == Color::P2) k2 = Pos(-1);
        }
        pieces[i] = p;
        if(p.type() == PieceType::King) {
            if(p.color() == Color::P1) k1 = Pos(i);
            if(p.color() == Color::P2) k2 = Pos(i);
        }
    }

    bool operator==(const Board& other) const {
        return (k1 == other.k1 && k2 == other.k2 && pieces == other.pieces);
    }

    constexpr uint8_t king1() const { return k1.idx() < rows*cols ? k1.idx() : 0; }
    constexpr uint8_t king2() const { return k2.idx() < rows*cols ? k2.idx() : 0; }

private:

    void check() const {
        if(k1.idx() < rows*cols) assert(pieces[k1.idx()].type() == PieceType::King);
        if(k2.idx() < rows*cols) assert(pieces[k2.idx()].type() == PieceType::King);
    }

    Pos k1;
    Pos k2;
    std::array<Piece, rows*cols> pieces;

};

#endif