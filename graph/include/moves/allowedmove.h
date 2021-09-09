#ifndef ALLOWEDMOVE_H
#define ALLOWEDMOVE_H

#include "staticvector.h"
#include "piece.h"
#include <cstdint>
#include <algorithm>
#include <cassert>

// for rows = 4, cols = 3
// 0  1  2
// 3  4  5
// 6  7  8
// 9  10 11

constexpr int iabs(int z) {
    return (z >= 0 ? z : -z);
}


static constexpr unsigned int rows = 4;
static constexpr unsigned int cols = 3;

using move_set = static_vector<uint8_t, 8>;
using move_sets = static_vector<move_set, rows*cols>;

static constexpr move_sets computeMoveSets(Piece p) {
    move_sets moveSet;
    for(size_t i = 0; i < rows*cols; ++i) moveSet.emplace_back();

    auto king_valid_move = [](int xsrc, int ysrc, int xdst, int ydst) {
        return (xsrc != xdst || ysrc != ydst) && 
                ( std::max(iabs(xsrc-xdst), iabs(ysrc-ydst)) == 1 );
    };

    auto tower_valid_move = [](int xsrc, int ysrc, int xdst, int ydst) {
        return (xsrc != xdst || ysrc != ydst) &&
                ( std::max(iabs(xsrc-xdst), iabs(ysrc-ydst)) == 1 ) &&
                ((xsrc-xdst) * (ysrc-ydst) == 0);
    };

    auto bishop_valid_move = [](int xsrc, int ysrc, int xdst, int ydst) {
        return (xsrc != xdst || ysrc != ydst) &&
                ( std::max(iabs(xsrc-xdst), iabs(ysrc-ydst)) == 1 ) &&
                ((xsrc-xdst) * (ysrc-ydst) != 0);
    };

    auto pawn_valid_move = [&](int xsrc, int ysrc, int xdst, int ydst) {
        if(isPlayer0(p)) {
            return xsrc == xdst && ydst == ysrc+1;
        }
        if(isPlayer1(p)) {
            return xsrc == xdst && ydst == ysrc-1;
        }
        return false;
    };

    auto queen_valid_move = [&](int xsrc, int ysrc, int xdst, int ydst) {
        if(isPlayer0(p)) {
            return (ydst >= ysrc && std::max(iabs(xsrc-xdst), iabs(ysrc-ydst)) == 1) || (ydst == ysrc-1 && xsrc == xdst);
        }
        if(isPlayer1(p)) {
            return (ydst <= ysrc && std::max(iabs(xsrc-xdst), iabs(ysrc-ydst)) == 1) || (ydst == ysrc+1 && xsrc == xdst);
        }
        return false;
    };

    int i = 0;
    for(int i = 0; i < rows*cols; ++i) {
        move_set& movesAtPos = moveSet[i];
        int xsrc = i%cols;
        int ysrc = i/cols;
        for(int j = 0; j < rows*cols; ++j) {
            int xdst = j%cols;
            int ydst = j/cols;
            if(isKing(p) && king_valid_move(xsrc, ysrc, xdst, ydst)) {
                movesAtPos.emplace_back(j);
            }
            if(isRook(p) & tower_valid_move(xsrc, ysrc, xdst, ydst)) {
                movesAtPos.emplace_back(j);
            }
            if(isBishop(p)  && bishop_valid_move(xsrc, ysrc, xdst, ydst)) {
                movesAtPos.emplace_back(j);
            }
            if(isPawn(p)  && pawn_valid_move(xsrc, ysrc, xdst, ydst)) {
                movesAtPos.emplace_back(j);
            }
            if(isQueen(p)  && queen_valid_move(xsrc, ysrc, xdst, ydst)) {
                movesAtPos.emplace_back(j);
            }
        }
    }

    return moveSet;
}

static constexpr static_vector<move_sets, 11> allMoveSets {
    computeMoveSets(Piece::K),
    computeMoveSets(Piece::k),
    computeMoveSets(Piece::P),
    computeMoveSets(Piece::p),
    computeMoveSets(Piece::B),
    computeMoveSets(Piece::b),
    computeMoveSets(Piece::R),
    computeMoveSets(Piece::r),
    computeMoveSets(Piece::Q),
    computeMoveSets(Piece::q),
    computeMoveSets(Piece::E),
};

static constexpr const move_sets& moveSets(Piece piece) {
    return allMoveSets[static_cast<char>(piece)];
}

static constexpr const move_set& moveSet(Piece piece, uint8_t position) {
    return allMoveSets[static_cast<char>(piece)][position];
}

#ifndef NDEBUG
namespace {

    using move_set = static_vector<uint8_t, 8>;
    using move_sets = static_vector<move_set, 12>;

    static constexpr move_sets empty { 
        {}, {}, {}, 
        {}, {}, {}, 
        {}, {}, {}, 
        {}, {}, {}
    };

    static constexpr move_sets king { 
        { 1, 3, 4 },       { 0, 2, 3, 4, 5 },           { 1, 4, 5 },
        { 0, 1, 4, 6, 7 }, { 0, 1, 2, 3, 5, 6, 7, 8 },  { 1, 2, 4 ,7, 8 },
        { 3, 4, 7, 9, 10}, { 3, 4, 5, 6, 8, 9, 10, 11}, { 4, 5, 7, 10, 11 },
        { 6, 7, 10},       { 6, 7, 8, 9, 11 },          { 7, 8, 10 }
    };
    static constexpr move_sets tower { 
        { 1, 3 },    { 0, 2, 4 },    { 1, 5 },
        { 0, 4, 6 }, { 1, 3, 5, 7 }, { 2, 4, 8 },
        { 3, 7, 9 }, { 4, 6, 8, 10}, { 5, 7, 11 },
        { 6, 10 },   { 7, 9, 11 },   { 8, 10 }
    };
    static constexpr move_sets bishop { 
        { 4 },    { 3, 5 },       { 4 },
        { 1, 7 }, { 0, 2, 6, 8 }, { 1, 7 },
        { 4, 10 },{ 3, 5, 9, 11}, { 4, 10},
        { 7 },    { 6, 8 },       { 7 }
    };
    static constexpr move_sets p0Pawn { 
        { 3 }, { 4 }, { 5 },
        { 6 }, { 7 }, { 8 },
        { 9 }, { 10 }, { 11 },
        { }, { }, { }
    };
    static constexpr move_sets p1Pawn { 
        { }, { }, { },
        { 0 }, { 1 }, { 2 },
        { 3 }, { 4 }, { 5 },
        { 6 }, { 7 }, { 8 }
    };
    static constexpr move_sets p0Queen { 
        { 1, 3, 4 },    { 0, 2, 3, 4, 5 },     { 1, 4, 5 },
        { 0, 4, 6, 7 }, { 1, 3, 5, 6, 7, 8 },  { 2, 4 ,7, 8 },
        { 3, 7, 9, 10}, { 4, 6, 8, 9, 10, 11}, { 5, 7, 10, 11 },
        { 6, 10},       { 7, 9, 11 },          { 8, 10 }
    };
    static constexpr move_sets p1Queen { 
        { 1, 3 },       { 0, 2, 4 },          { 1, 5 },
        { 0, 1, 4, 6 }, { 0, 1, 2, 3, 5, 7 }, { 1, 2, 4, 8 },
        { 3, 4, 7, 9 }, { 3, 4, 5, 6, 8, 10}, { 4, 5, 7, 11 },
        { 6, 7, 10},    { 6, 7, 8, 9, 11 },   { 7, 8, 10 }
    };


    static_assert(empty   == computeMoveSets(Piece::E));
    static_assert(king    == computeMoveSets(Piece::K));
    static_assert(king    == computeMoveSets(Piece::k));
    static_assert(tower   == computeMoveSets(Piece::R));
    static_assert(tower   == computeMoveSets(Piece::r));
    static_assert(bishop  == computeMoveSets(Piece::B));
    static_assert(bishop  == computeMoveSets(Piece::b));
    static_assert(p0Pawn  == computeMoveSets(Piece::P));
    static_assert(p1Pawn  == computeMoveSets(Piece::p));
    static_assert(p0Queen == computeMoveSets(Piece::Q));
    static_assert(p1Queen == computeMoveSets(Piece::q));

}
#endif

#endif