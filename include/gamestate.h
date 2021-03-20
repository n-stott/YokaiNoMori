#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "action.h"
#include "board.h"
#include "piece.h"
#include "reserve.h"

#include <array>
#include <string>
#include <vector>
#include <iostream>

struct GameState {
    Board board;
    Reserve<P1> reserve1;
    Reserve<P2> reserve2;
    Color currentPlayer;

    GameState() : 
        board(),
        reserve1(),
        reserve2(),
        currentPlayer(P1)
    { }

    std::string toString() const;
    std::string niceToString() const;
    ActionSet allowedActions() const;

    bool apply(Action action) {
        if(action.type == Move) {
            return move(action.p.type(), action.p.color(), action.src, action.dst);
        }
        else {
            return drop(action.p.type(), action.p.color(), action.reservePos, action.dst);
        }
    }
    
public:

    bool allowedMove(PieceType pt, Color c, Pos a, Pos b) const;
    bool allowedDrop(PieceType pt, Color c, uint8_t posInReserve, Pos a) const;
    bool move(PieceType pt, Color c, Pos a, Pos b);
    bool drop(PieceType pt, Color c, uint8_t posInReserve, Pos a);
    static bool allowedOffset(PieceType pt, Color c, Pos a, Pos b);

    bool hasWon(Color player) const;
    bool hasLost(Color player) const;

    bool hasWinner() const { return hasWon(Color::P1) || hasWon(Color::P2); }

    void swapPlayer() {
        currentPlayer = (currentPlayer == P1 ? P2 : P1);
    }

};


static_assert(sizeof(GameState) == 27);

#endif