#ifndef STATE_H
#define STATE_H

#include "action.h"
#include "board.h"
#include "piece.h"
#include "reserve.h"

#include <array>
#include <string>
#include <vector>
#include <iostream>

struct State {
    Board board;
    Reserve<P1> reserve1;
    Reserve<P2> reserve2;
    Color currentPlayer;

    State() : 
        board(),
        reserve1(),
        reserve2(),
        currentPlayer(P1)
    { }

    std::string toString() const;
    std::string niceToString() const;

    bool allowedMove(PieceType pt, Color c, Pos a, Pos b) const;
    bool allowedDrop(PieceType pt, Color c, uint8_t posInReserve, Pos a) const;
    void move(PieceType pt, Color c, Pos a, Pos b);
    void drop(PieceType pt, Color c, uint8_t posInReserve, Pos a);
    static bool allowedOffset(PieceType pt, Color c, Pos a, Pos b);
    ActionSet allowedActions() const;

    
    

private:

    void swapPlayer() {
        currentPlayer = (currentPlayer == P1 ? P2 : P1);
    }

};


static_assert(sizeof(State) == 27);
static_assert(sizeof(State) <= 32); // fit two states on a cache line ?

#endif