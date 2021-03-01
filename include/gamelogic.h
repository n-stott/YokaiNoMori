#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "state.h"
#include <array>
#include <algorithm>

class GameLogic {

    static bool valid(const State& state) {
        std::array<short, BASE_TYPES> pieces1;
        pieces1.fill(0);
        std::array<short, BASE_TYPES> pieces2;
        pieces2.fill(0);
        for(const Piece& piece : state.board) {
            if(piece.color == P1) ++pieces1[piece.type & BASE_MASK];
            if(piece.color == P2) ++pieces2[piece.type & BASE_MASK];
        }
        for(const Piece& piece : state.r1) ++pieces1[piece.type & BASE_MASK];
        for(const Piece& piece : state.r2) ++pieces2[piece.type & BASE_MASK];
        return std::all_of(pieces1.begin(), pieces1.end(), [](short val) { return val == 2; })
            && std::all_of(pieces2.begin(), pieces2.end(), [](short val) { return val == 2; });
    }



};


#endif