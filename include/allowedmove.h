#ifndef ALLOWEDMOVE_H
#define ALLOWEDMOVE_H

#include "staticvector.h"
#include "pos.h"
#include "enums.h"
#include "gameconfig.h"
#include <cstdint>
#include <algorithm>

// for rows = 4, cols = 3
// 0  1  2
// 3  4  5
// 6  7  8
// 9  10 11

template<BoardConfig config>
struct AllowedMove {

    static constexpr unsigned int rows = GameConfig<config>::rows;
    static constexpr unsigned int cols = GameConfig<config>::cols;

    using move_set = static_vector<Pos, 8>;
    using move_sets = static_vector<move_set, rows*cols>;

    static constexpr move_sets computeMoveSets(PieceType pt, Color player) {
        move_sets moveSet;
        for(size_t i = 0; i < rows*cols; ++i) moveSet.emplace_back();

        auto king_valid_move = [](int xsrc, int ysrc, int xdst, int ydst) {
            return (xsrc != xdst || ysrc != ydst) && 
                   ( std::max(std::abs(xsrc-xdst), std::abs(ysrc-ydst)) == 1 );
        };

        auto tower_valid_move = [](int xsrc, int ysrc, int xdst, int ydst) {
            return (xsrc != xdst || ysrc != ydst) &&
                   ( std::max(std::abs(xsrc-xdst), std::abs(ysrc-ydst)) == 1 ) &&
                   ((xsrc-xdst) * (ysrc-ydst) == 0);
        };

        auto bishop_valid_move = [](int xsrc, int ysrc, int xdst, int ydst) {
            return (xsrc != xdst || ysrc != ydst) &&
                   ( std::max(std::abs(xsrc-xdst), std::abs(ysrc-ydst)) == 1 ) &&
                   ((xsrc-xdst) * (ysrc-ydst) != 0);
        };

        auto pawn_valid_move = [&](int xsrc, int ysrc, int xdst, int ydst) {
            if(player == Color::P1) {
                return xsrc == xdst && ydst == ysrc-1;
            }
            if(player == Color::P2) {
                return xsrc == xdst && ydst == ysrc+1;
            }
            return false;
        };

        auto superpawn_valid_move = [&](int xsrc, int ysrc, int xdst, int ydst) {
            if(player == Color::P1) {
                return (ydst <= ysrc && std::max(std::abs(xsrc-xdst), std::abs(ysrc-ydst)) == 1) || (ydst == ysrc+1 && xsrc == xdst);
            }
            if(player == Color::P2) {
                return (ydst >= ysrc && std::max(std::abs(xsrc-xdst), std::abs(ysrc-ydst)) == 1) || (ydst == ysrc-1 && xsrc == xdst);
            }
            return false;
        };

        auto archbishop_valid_move = [&](int xsrc, int ysrc, int xdst, int ydst) {
            return bishop_valid_move(xsrc, ysrc, xdst, ydst) || pawn_valid_move(xsrc, ysrc, xdst, ydst);
        };

        int i = 0;
        for(int i = 0; i < rows*cols; ++i) {
            move_set& movesAtPos = moveSet[i];
            int xsrc = i%cols;
            int ysrc = i/cols;
            for(int j = 0; j < rows*cols; ++j) {
                int xdst = j%cols;
                int ydst = j/cols;
                if(pt == PieceType::King && king_valid_move(xsrc, ysrc, xdst, ydst)) {
                    movesAtPos.emplace_back(j);
                }
                if(pt == PieceType::Tower && tower_valid_move(xsrc, ysrc, xdst, ydst)) {
                    movesAtPos.emplace_back(j);
                }
                if(pt == PieceType::Bishop && bishop_valid_move(xsrc, ysrc, xdst, ydst)) {
                    movesAtPos.emplace_back(j);
                }
                if(pt == PieceType::Pawn && pawn_valid_move(xsrc, ysrc, xdst, ydst)) {
                    movesAtPos.emplace_back(j);
                }
                if(pt == PieceType::SuperPawn && superpawn_valid_move(xsrc, ysrc, xdst, ydst)) {
                    movesAtPos.emplace_back(j);
                }
                if(pt == PieceType::ArchBishop && archbishop_valid_move(xsrc, ysrc, xdst, ydst)) {
                    movesAtPos.emplace_back(j);
                }
            }
        }

        return moveSet;
    }

};

#ifndef NDEBUG
namespace {

    using move_set = static_vector<Pos, 8>;
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
    static constexpr move_sets p1Pawn { 
        { }, { }, { },
        { 0 }, { 1 }, { 2 },
        { 3 }, { 4 }, { 5 },
        { 6 }, { 7 }, { 8 }
    };
    static constexpr move_sets p2Pawn { 
        { 3 }, { 4 }, { 5 },
        { 6 }, { 7 }, { 8 },
        { 9 }, { 10 }, { 11 },
        { }, { }, { }
    };
    static constexpr move_sets p1SuperPawn { 
        { 1, 3 },       { 0, 2, 4 },          { 1, 5 },
        { 0, 1, 4, 6 }, { 0, 1, 2, 3, 5, 7 }, { 1, 2, 4, 8 },
        { 3, 4, 7, 9 }, { 3, 4, 5, 6, 8, 10}, { 4, 5, 7, 11 },
        { 6, 7, 10},    { 6, 7, 8, 9, 11 },   { 7, 8, 10 }
    };
    static constexpr move_sets p2SuperPawn { 
        { 1, 3, 4 },    { 0, 2, 3, 4, 5 },     { 1, 4, 5 },
        { 0, 4, 6, 7 }, { 1, 3, 5, 6, 7, 8 },  { 2, 4 ,7, 8 },
        { 3, 7, 9, 10}, { 4, 6, 8, 9, 10, 11}, { 5, 7, 10, 11 },
        { 6, 10},       { 7, 9, 11 },          { 8, 10 }
    };


    static_assert(empty       == AllowedMove<Easy>::computeMoveSets(PieceType::NoType   , Color::P1));
    static_assert(empty       == AllowedMove<Easy>::computeMoveSets(PieceType::NoType   , Color::P2));
    static_assert(king        == AllowedMove<Easy>::computeMoveSets(PieceType::King     , Color::P1));
    static_assert(king        == AllowedMove<Easy>::computeMoveSets(PieceType::King     , Color::P2));
    static_assert(tower       == AllowedMove<Easy>::computeMoveSets(PieceType::Tower    , Color::P1));
    static_assert(tower       == AllowedMove<Easy>::computeMoveSets(PieceType::Tower    , Color::P2));
    static_assert(bishop      == AllowedMove<Easy>::computeMoveSets(PieceType::Bishop   , Color::P1));
    static_assert(bishop      == AllowedMove<Easy>::computeMoveSets(PieceType::Bishop   , Color::P2));
    static_assert(p1Pawn      == AllowedMove<Easy>::computeMoveSets(PieceType::Pawn     , Color::P1));
    static_assert(p2Pawn      == AllowedMove<Easy>::computeMoveSets(PieceType::Pawn     , Color::P2));
    static_assert(p1SuperPawn == AllowedMove<Easy>::computeMoveSets(PieceType::SuperPawn, Color::P1));
    static_assert(p2SuperPawn == AllowedMove<Easy>::computeMoveSets(PieceType::SuperPawn, Color::P2));

}
#endif

#endif