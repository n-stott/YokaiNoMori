#ifndef GAMEHISTORY_H
#define GAMEHISTORY_H

#include "staticvector.h"
#include "board.h"
#include <vector>


struct GameHistory {

    std::vector<board_hash_t> positions;

    GameHistory() { }

    template<typename Board>
    void push(const Board& board) {
        positions.push_back(board.hash());
    }

    void pop() {
        positions.pop_back();
    }

    unsigned int count(board_hash_t hash) const {
        unsigned int val = 0;
        for(board_hash_t h : positions) {
            val += (h == hash);
        }
        return val;
    }

    bool hasDraw() const {
        // We only need to check if the last move was a draw
        if(positions.begin() == positions.end()) return false;
        board_hash_t drawHash = positions.back();
        unsigned int val = 0;
        for (auto it = positions.rbegin(); it != positions.rend(); ++it) {
            val += (*it == drawHash);
        }
        return val == 3;
    }

};


#endif