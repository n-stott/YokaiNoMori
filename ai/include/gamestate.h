#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "action.h"
#include "board.h"
#include "piece.h"
#include "reserve.h"
#include "gamehistory.h"
#include "gameconfig.h"
#include "constants.h"

#include <array>
#include <string>
#include <minimax/logger.h>

struct GameState {

    static constexpr unsigned int rows = GameConfig::rows;
    static constexpr unsigned int cols = GameConfig::cols;
    static constexpr unsigned int ressize = GameConfig::ressize;

    GameHistory* history;
    Board board;
    Reserve<P0, ressize> reserve0;
    Reserve<P1, ressize> reserve1;
    Color currentPlayer;
    Color winner;
    uint8_t nbTurns;
    uint8_t maxTurns;

    GameState(GameHistory* history) : 
        history(history),
        board(),
        reserve0(),
        reserve1(),
        currentPlayer(P0),
        winner(None),
        nbTurns(0),
        maxTurns(MAX_TURNS)
    {
        if(history) history->push(board);
    }


    GameState(GameHistory* history, 
            Board b,
            Reserve<P0, GameConfig::ressize> r0,
            Reserve<P1, GameConfig::ressize> r1,
            Color player) :
        history(history),
        board(b),
        reserve0(r0),
        reserve1(r1),
        currentPlayer(player),
        winner(None),
        nbTurns(0),
        maxTurns(MAX_TURNS)
    {
        if(history) history->push(board);
        if(hasWon(P0)) winner = P0;
        if(hasWon(P1)) winner = P1;
    }

    GameState(GameHistory* history, const char* sboard, const std::string& sres0, const std::string& sres1, Color player) :
        history(history),
        board(sboard),
        reserve0(sres0),
        reserve1(sres1),
        currentPlayer(player),
        winner(None),
        nbTurns(0),
        maxTurns(150)
    {
        if(history) history->push(board);
        if(hasWon(P0)) winner = P0;
        if(hasWon(P1)) winner = P1;
    }


    inline std::string toString() const {
        std::string s;
        s += board.toString();
        s += '|';
        s += reserve0.toString();
        s += '|';
        s += reserve1.toString();
        return s;
    }

    inline std::string toString2() const {
        char pieceCode[2*NB_PIECE_TYPE] = {
            'v', // P1 NoType
            'v', // P2 NoType
            'a', // P1 King
            'f', // P2 King
            'c', // P1 Tower
            'h', // P2 Tower
            'b', // P1 Bishop
            'g', // P2 Bishop
            'd', // P1 Pawn
            'i', // P2 Pawn
            'e', // P1 SuperPawn
            'j', // P2 SuperPawn
        };
        std::string b;
        for(Piece p : board) b += pieceCode[p.id()];
        std::string r0("vvvvvvvv");
        std::string r1("vvvvvvvv");
        for(unsigned int i = 0; i < reserve0.size; ++i) 
            r0[i] = pieceCode[reserve0[i].id()];
        for(unsigned int i = 0; i < reserve1.size; ++i) 
            r1[i] = pieceCode[reserve1[i].id()];

        std::string s = b + " " + r0 + " " + r1;
        return s;
    }

    inline std::string niceToString() const {
        std::string s;
        s += "-----------------\n";
        s += (currentPlayer == P1 ? "> " : "  ");
        s += "Player b | ";
        s += reserve1.toString();
        s += '\n';
        s += "-----------------";
        for(uint8_t i = rows; i --> 0;) {
            s += '\n' + std::to_string(i+1) + ' ';
            for(uint8_t j = 0; j < cols; ++j) {
                s += board.get(3*i+j).toChar();
            }
        }
        s += '\n';
        s += "  ABC\n";
        s += "-----------------\n";
        s += (currentPlayer == P0 ? "> " : "  ");
        s += "Player A | ";
        s += reserve0.toString();
        s += '\n';
        s += "-----------------";
        return s;
    }

    void fillAllowedActions(ActionSet*) const;

    bool apply(Action action) {
        assert(winner == None);
        assert(checkAction(action));
        bool res = false;
        if(action.type == Move) {
            res = move(action.p, action.src, action.dst);
        }
        else {
            res = drop(action.p, action.src, action.dst);
        }
        if(res) {
            if(history) history->push(board);
            // player wins if he ate king, so they win if a king is at the top of their reserve
            assert(action.p.color() != None);
            if(action.p.color() == P0) {
                if(reserve0.size > 0 && reserve0[reserve0.size-1].type() == King) winner = P0;
            } else {
                if(reserve1.size > 0 && reserve1[reserve1.size-1].type() == King) winner = P1;
            }
        }
        return res;
    }

    void revert() {
        if(history) history->pop();
        winner = None;
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
    bool allowedDrop(Piece p, uint8_t posInReserve, Pos a) const;
    bool move(Piece p, Pos a, Pos b);
    bool drop(Piece p, Pos res, Pos dst);
    static bool allowedOffset(Piece p, Pos a, Pos b);

    inline bool hasWon(Color player) const { return winner == player; }

    inline bool hasLost(Color player) const {
        if(player == Color::P0) {
            return hasWon(Color::P1);
        } else {
            return hasWon(Color::P0);
        }
    }

    bool gameOver() const {
        return hasWinner() || hasDraw();
    }

    bool hasWinner() const { return winner != None; }

    bool hasDraw() const {
        return (nbTurns == maxTurns) || (history && history->hasDraw());
    }

    void swapPlayer() {
        currentPlayer = (currentPlayer == P0 ? P1 : P0);
        ++nbTurns;
    }

};


#endif
