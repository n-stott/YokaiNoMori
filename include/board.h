#ifndef BOARD_H
#define BOARD_H

#include "piece.h"
#include "enums.h"
#include <string>
#include <algorithm>

struct Board {

    Board() :
        pieces({
            Piece(Tower, P2), Piece(King, P2), Piece(Rook, P2),
            Piece(),          Piece(Pawn, P2), Piece(),
            Piece(),          Piece(Pawn, P1), Piece(),
            Piece(Rook, P1),  Piece(King, P1), Piece(Tower, P1)
        })
    { }

    Board(const std::string& board) {
        for(size_t i = 0; i < std::min(board.size(), 12ul); ++i) {
            pieces[i] = Piece(board[i]);
        }
    }

    const Piece* begin() const noexcept { return pieces.begin(); }
    const Piece* end() const noexcept { return pieces.end(); }

    const Piece& operator[](int i) const { return pieces[i]; }
    Piece& operator[](int i) { return pieces[i]; }

private:
    std::array<Piece, 12> pieces;

};

#endif