#ifndef BOARD_H
#define BOARD_H

#include "piece.h"
#include "enums.h"
#include <algorithm>
#include <cstring>

struct Board {

    Board() :
        pieces({
            Piece(Bishop, P2), Piece(King, P2), Piece(Tower, P2),
            Piece(),           Piece(Pawn, P2), Piece(),
            Piece(),           Piece(Pawn, P1), Piece(),
            Piece(Tower, P1),  Piece(King, P1), Piece(Bishop, P1)
        })
    { }

    Board(const char* board) {
        assert(std::strlen(board) == 12);
        for(size_t i = 0; i < 12; ++i) {
            pieces[i] = Piece(board[i]);
        }
    }

    const Piece* begin() const noexcept { return pieces.begin(); }
    const Piece* end() const noexcept { return pieces.end(); }

    Piece get(uint8_t i) const { return pieces[i]; }
    void  set(uint8_t i, Piece p) { pieces[i] = p; }

    using hash_t = unsigned int;

    hash_t hash() const {
        hash_t value = 0;
        for(unsigned int i = 0; i < 12; ++i) value = (i > 0 ? 6 : 1) * value + static_cast<int>(pieces[i].type());
        return value;
    }

private:
    std::array<Piece, 12> pieces;

};

#endif