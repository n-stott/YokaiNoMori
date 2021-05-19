#ifndef GAMEHISTORY_H
#define GAMEHISTORY_H

#include "staticvector.h"
#include "board.h"

struct GameHistory {

    static_vector<Board::hash_t, 50> positions;

    GameHistory() { }

    void record(const Board& board) {
        positions.push_back(board.hash());
    }

    unsigned int count(Board::hash_t hash) const {
        unsigned int val = 0;
        for(Board::hash_t h : positions) {
            val += (h == hash);
        }
        return val;
    }

};


#endif