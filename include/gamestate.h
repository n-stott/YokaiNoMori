#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "action.h"
#include "board.h"
#include "piece.h"
#include "reserve.h"
#include "gamehistory.h"

#include <array>
#include <string>
#include <minimax/logger.h>

template<BoardConfig config>
struct GameConfig;

template<>
struct GameConfig<BoardConfig::Easy> {
    static constexpr unsigned int rows = 4;
    static constexpr unsigned int cols = 3;
    static constexpr unsigned int ressize = 7;
};

template<>
struct GameConfig<BoardConfig::Medium> {
    static constexpr unsigned int rows = 5;
    static constexpr unsigned int cols = 6;
    static constexpr unsigned int ressize = 15;
};

template<BoardConfig config>
struct GameState {

    static constexpr unsigned int rows = GameConfig<config>::rows;
    static constexpr unsigned int cols = GameConfig<config>::cols;
    static constexpr unsigned int ressize = GameConfig<config>::ressize;

    Board<rows, cols> board;
    Reserve<P1, ressize> reserve1;
    Reserve<P2, ressize> reserve2;
    Color currentPlayer;
    Color winner;
    uint8_t nbTurns;
    uint8_t maxTurns;
    GameHistory* history;

    GameState(GameHistory* history) : 
        board(),
        reserve1(),
        reserve2(),
        currentPlayer(P1),
        winner(None),
        nbTurns(0),
        maxTurns(150),
        history(history)
    { }

    GameState(GameHistory* history, const char* sboard, const std::string& sres1, const std::string& sres2, Color player) :
        board(sboard),
        reserve1(sres1),
        reserve2(sres2),
        currentPlayer(player),
        winner(None),
        nbTurns(0),
        maxTurns(150),
        history(history)
    {
        if(hasWon(P1)) winner = P1;
        if(hasWon(P2)) winner = P2;
    }


    inline std::string toString() const {
        std::string s;
        for(Piece p : board) s+=p.toChar();
        s += '|';
        s += reserve1.toString();
        s += '|';
        s += reserve2.toString();
        return s;
    }

    inline std::string niceToString() const {
        std::string s;
        s += "-----------------\n";
        s += (currentPlayer == P2 ? "> " : "  ");
        s += "Player B | ";
        s += reserve2.toString();
        s += '\n';
        s += "-----------------";
        for(uint8_t i = 0; i < 12; ++i) {
            if(i%3 == 0) s += '\n' + std::to_string(1+i/3) + ' ';
            s += board.get(i).toChar();
        }
        s += '\n';
        s += "  ABC\n";
        s += "-----------------\n";
        s += (currentPlayer == P1 ? "> " : "  ");
        s += "Player a | ";
        s += reserve1.toString();
        s += '\n';
        s += "-----------------";
        return s;
    }

    void fillAllowedActions(ActionSet*) const;

    bool apply(Action action) {
        assert(checkAction(action));
        bool res = false;
        if(action.type == Move) {
            res = move(action.p, action.src, action.dst);
        }
        else {
            res = drop(action.p, action.src, action.dst);
        }
        if(res) {
            history->push(board);
            // player wins if he ate king, so they win if a king is at the top of their reserve
            assert(action.p.color() != None);
            if(action.p.color() == P1) {
                if(reserve1.size > 0 && reserve1[reserve1.size-1].type() == King) winner = P1;
            } else {
                if(reserve2.size > 0 && reserve2[reserve2.size-1].type() == King) winner = P2;
            }
        }
        return res;
    }

    void revert() {
        history->pop();
    }
    
public:

    bool checkAction(Action action) const {
        if(action.type == Move) {
            return checkMove(action.p, action.src, action.dst);
        }
        else {
            return checkDrop(action.p, action.src, action.dst);
        }
    }

    bool checkMove(Piece p, Pos a, Pos b) const;
    bool checkDrop(Piece p, Pos res, Pos dst) const;

    bool allowedMove(Piece p, Pos a, Pos b) const;
    bool allowedDrop(Piece p, uint8_t& posInReserve, Pos a) const;
    bool move(Piece p, Pos a, Pos b);
    bool drop(Piece p, Pos res, Pos dst);
    static bool allowedOffset(Piece p, Pos a, Pos b);

    inline bool hasWon(Color player) const { return winner == player; }

    inline bool hasLost(Color player) const {
        if(player == Color::P1) {
            return hasWon(Color::P2);
        } else {
            return hasWon(Color::P1);
        }
    }

    bool gameOver() const {
        return hasWinner() || hasDraw();
    }

    bool hasWinner() const { return winner != None; }

    bool hasDraw() const {
        return (nbTurns == maxTurns) || history->hasDraw();
    }

    void swapPlayer() {
        currentPlayer = (currentPlayer == P1 ? P2 : P1);
        ++nbTurns;
    }

};



// static_assert(sizeof(GameState) == 40);


// A piece can have at most 15 different positions (12 on board, one for each reserve, one for nonexistent (SP))
// Hence the position for a piece can be stored on 4 bits
// The color requires a 5th bit
// the current player requires another bit
// The whole state then fits in 5*10+1 bit

// To avoid symetries, the storage is done as follows : 
// King with least index, King with largest index, Tower with least index, ...
// with indices 0 = does not exist, 1-12 = positions 0-11, 13 = reserve1, 14 = reserve2 

struct GameStateHash {

    template<typename GameState>
    GameStateHash(const GameState& state) : value(0) {
        short kings = 0, towers = 0, bishops = 0, pawns = 0, superpawns = 0;
        auto pieceHash = [&](Piece p, short position) {
            short value = 2*position+(p.color() == P1);
            if(p.type() == King)      { value <<= 4*(2*4+kings); ++kings; }
            if(p.type() == Tower)     { value <<= 4*(2*3+towers); ++towers; }
            if(p.type() == Bishop)    { value <<= 4*(2*2+bishops); ++bishops; }
            if(p.type() == Pawn)      { value <<= 4*(2*1+pawns); ++pawns; }
            if(p.type() == SuperPawn) { value <<= 4*(2*0+superpawns); ++superpawns; }
            return value;
        };
        for(uint8_t i = 0; i < 12; ++i) {
            value |= pieceHash(state.board.get(i), 1+i);
        }
        for(uint8_t i = 0; i < state.reserve1.size; ++i) {
            value |= pieceHash(state.reserve1[i], 13);
        }
        for(uint8_t i = 0; i < state.reserve2.size; ++i) {
            value |= pieceHash(state.reserve2[i], 14);
        }
        value = 2*value + (state.currentPlayer == P1);
    }

    unsigned long long value;
};

#endif
