#ifndef GAMEHISTORY_H
#define GAMEHISTORY_H

#include "gamelogic.h"
#include "staticvector.h"
#include "board.h"
#include "constants.h"
#include <vector>


template<BoardConfig config>
struct GameHistory {

    using board_t = Board<GameLogic<config>::rows, GameLogic<config>::cols>;

    static constexpr unsigned int nbKingPos = GameLogic<config>::rows*GameLogic<config>::cols;

    static_vector<board_t, MAX_TURNS+2> positions;
    std::array<uint8_t, nbKingPos*nbKingPos> countKingPos;
    bool cached_draw;
    bool cache_valid;


    GameHistory() :
        cached_draw(false),
        cache_valid(false)
    {
        std::fill(countKingPos.begin(), countKingPos.end(), 0);
    }

    
    void push(const board_t& board) {
        positions.push_back(board);
        ++countKingPos[board.king1()*nbKingPos+board.king2()];
        cache_valid = false;
    }

    void pop() {
        const board_t& board = positions.back();
        --countKingPos[board.king1()*nbKingPos+board.king2()];
        positions.pop_back();
        cache_valid = false;
    }

    bool hasDraw() {
        if(cache_valid) return cached_draw;
        // We only need to check if the last move was a draw
        if(positions.begin() == positions.end()) {
            set_cache(false);
            return false;
        }
        const board_t& h = positions.back();
        if(countKingPos[h.king1()*nbKingPos+h.king2()] < 3) {
            set_cache(false);
            return false;
        }
        unsigned int val = 0;
        for (auto& b : positions) {
            val += (b == h);
        }
        set_cache(val == 3);
        return val == 3;
    }

    void set_cache(bool v) {
        cached_draw = v;
        cache_valid = true;
    }

};


#endif