#ifndef BOARD_H
#define BOARD_H

#include "piece.h"
#include "enums.h"
#include <algorithm>
#include <cstring>

using board_hash_t = unsigned int;

template<unsigned int rows, unsigned int cols>
struct Board {

    template<unsigned int R = rows, unsigned int C = cols> requires(R == 4 && C == 3)
    Board() :
        pieces({
            Piece(Bishop, P2), Piece(King, P2), Piece(Tower, P2),
            Piece(),           Piece(Pawn, P2), Piece(),
            Piece(),           Piece(Pawn, P1), Piece(),
            Piece(Tower, P1),  Piece(King, P1), Piece(Bishop, P1)
        })
    { }


    template<unsigned int R = rows, unsigned int C = cols> requires(R == 6 && C == 5)
    Board() :
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

    Piece get(uint8_t i) const { return pieces[i]; }
    void  set(uint8_t i, Piece p) { pieces[i] = p; }

    board_hash_t hash() const {
        board_hash_t value = 0;
        for(unsigned int i = 0; i < rows*cols; ++i) value = (i > 0 ? 6 : 1) * value + static_cast<int>(pieces[i].type());
        return value;
    }

private:
    std::array<Piece, rows*cols> pieces;

};

#endif