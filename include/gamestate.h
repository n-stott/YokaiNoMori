#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "action.h"
#include "board.h"
#include "piece.h"
#include "reserve.h"

#include <array>
#include <string>
#include <vector>

struct GameState {
    Board board;
    Reserve<P1> reserve1;
    Reserve<P2> reserve2;
    Color currentPlayer;
    uint8_t nbTurns;
    uint8_t maxTurns;

    GameState() : 
        board(),
        reserve1(),
        reserve2(),
        currentPlayer(P1),
        nbTurns(0),
        maxTurns(100)
    { }

    GameState(const std::string& sboard, const std::string& sres1, const std::string& sres2, Color player) :
        board(sboard),
        reserve1(sres1),
        reserve2(sres2),
        currentPlayer(player),
        nbTurns(0),
        maxTurns(100)
    { }

    std::string toString() const;
    std::string niceToString() const;
    void fillAllowedActions(ActionSet*) const;

    bool apply(Action action) {
        if(action.type == Move) {
            return move(action.p.type(), action.p.color(), action.src, action.dst);
        }
        else {
            return drop(action.p.type(), action.p.color(), action.dst);
        }
    }
    
public:

    bool allowedMove(PieceType pt, Color c, Pos a, Pos b) const;
    bool allowedDrop(PieceType pt, Color c, uint8_t& posInReserve, Pos a) const;
    bool move(PieceType pt, Color c, Pos a, Pos b);
    bool drop(PieceType pt, Color c, Pos a);
    static bool allowedOffset(PieceType pt, Color c, Pos a, Pos b);

    bool hasWon(Color player) const;
    bool hasLost(Color player) const;

    bool hasWinner() const { return hasWon(Color::P1) || hasWon(Color::P2); }

    void swapPlayer() {
        currentPlayer = (currentPlayer == P1 ? P2 : P1);
        ++nbTurns;
    }

};

static_assert(sizeof(GameState) == 29);


// A piece can have at most 15 different positions (12 on board, one for each reserve, one for nonexistent (SP))
// Hence the position for a piece can be stored on 4 bits
// The color requires a 5th bit
// the current player requires another bit
// The whole state then fits in 5*10+1 bit

// To avoid symetries, the storage is done as follows : 
// King with least index, King with largest index, Tower with least index, ...
// with indices 0 = does not exist, 1-12 = positions 0-11, 13 = reserve1, 14 = reserve2 

struct GameStateHash {

    GameStateHash(const GameState& state) : value(0) {
        short kings = 0, towers = 0, rooks = 0, pawns = 0, superpawns = 0;
        auto pieceHash = [&](Piece p, short position) {
            short value = 2*position+(p.color() == P1);
            if(p.type() == King)      { value <<= 4*(2*4+kings); ++kings; }
            if(p.type() == Tower)     { value <<= 4*(2*3+towers); ++towers; }
            if(p.type() == Rook)      { value <<= 4*(2*2+rooks); ++rooks; }
            if(p.type() == Pawn)      { value <<= 4*(2*1+pawns); ++pawns; }
            if(p.type() == SuperPawn) { value <<= 4*(2*0+superpawns); ++superpawns; }
            return value;
        };
        for(int i = 0; i < 12; ++i) {
            value |= pieceHash(state.board[i], 1+i);
        }
        for(int i = 0; i < state.reserve1.size; ++i) {
            value |= pieceHash(state.reserve1[i], 13);
        }
        for(int i = 0; i < state.reserve2.size; ++i) {
            value |= pieceHash(state.reserve2[i], 14);
        }
        value = 2*value + (state.currentPlayer == P1);
    }

    unsigned long long value;
};

#endif