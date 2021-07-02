#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "action.h"
#include "board.h"
#include "piece.h"
#include "reserve.h"
#include "gamehistory.h"
#include "gameconfig.h"

#include <array>
#include <string>
#include <minimax/logger.h>

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
    GameHistory<config>* history;

    GameState(GameHistory<config>* history) : 
        board(),
        reserve1(),
        reserve2(),
        currentPlayer(P1),
        winner(None),
        nbTurns(0),
        maxTurns(150),
        history(history)
    { }

    GameState(GameHistory<config>* history, const char* sboard, const std::string& sres1, const std::string& sres2, Color player) :
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
        for(uint8_t i = 0; i < rows*cols; ++i) {
            if(i%cols == 0) s += '\n' + std::to_string(1+i/cols) + ' ';
            s += board.get(i).toChar();
        }
        s += '\n';
        if(config == Easy) {
            s += "  ABC\n";
        } else {
            s += "  ABCDE\n";
        }
        s += "-----------------\n";
        s += (currentPlayer == P1 ? "> " : "  ");
        s += "Player a | ";
        s += reserve1.toString();
        s += '\n';
        s += "-----------------";
        return s;
    }

    void fillAllowedActions(ActionSet<config>*) const;

    bool apply(Action<config> action) {
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

    bool checkAction(Action<config> action) const {
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


#endif
