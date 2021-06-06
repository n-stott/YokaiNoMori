#ifndef ALLOWEDMOVE_H
#define ALLOWEDMOVE_H

#include "staticvector.h"
#include "pos.h"
#include "enums.h"
#include <cstdint>

// 0  1  2
// 3  4  5
// 6  7  8
// 9  10 11

struct AllowedMove {

    using move_set = static_vector<Pos, 8>;
    using move_sets = static_vector<move_set, 12>;

    static constexpr const move_sets& get(Color player, PieceType piece) {
        switch(piece) {
            case PieceType::King:
                return king;
            case PieceType::Bishop:
                return bishop;
            case PieceType::Tower:
                return tower;
            case PieceType::Pawn:
                if(player == Color::P1) return p1Pawn;
                else return p2Pawn;
            case PieceType::SuperPawn:
                if(player == Color::P1) return p1SuperPawn;
                else return p2SuperPawn;
            default:
                return empty;
        }
    }

    static constexpr move_sets empty { 
        {}, {}, {}, 
        {}, {}, {}, 
        {}, {}, {}, 
        {}, {}, {}
    };
    static constexpr move_sets king { 
        { 1, 3, 4 },       { 0, 2, 3, 4, 5 },           { 1, 4, 5 },
        { 0, 1, 4, 6, 7 }, { 0, 1, 2, 3, 5, 6, 7, 8 },  { 1, 2, 4 ,7, 8 },
        { 3, 4, 7, 9, 10}, { 3, 4, 5, 6, 8, 9, 10, 11}, { 4, 5, 7, 8, 10, 11 },
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
        { 3, 7, 9, 10}, { 4, 6, 8, 9, 10, 11}, { 5, 7, 8, 10, 11 },
        { 6, 10},       { 7, 9, 11 },          { 8, 10 }
    };
};



#endif