#ifndef BOARDANALYSIS_H
#define BOARDANALYSIS_H

#include "gamestate.h"
#include "gamelogic.h"
#include <bitset>
#include <cassert>
#include <array>

struct StateAnalysis {

    using mask = std::bitset<12>;

    mask occupied1;
    mask occupied2;

    mask controlled1;
    mask controlled2;

    mask kingPosition1;
    mask kingPosition2;

    mask kingControl1;
    mask kingControl2;

    std::array<short, 6> onBoard1;
    std::array<short, 6> onBoard2;

    std::array<short, 6> inReserve1;
    std::array<short, 6> inReserve2;


    template<BoardConfig config>
    StateAnalysis(const GameState<config>& state) :
        occupied1(),
        occupied2(),
        controlled1(),
        controlled2(),
        kingPosition1(),
        kingPosition2(),
        kingControl1(),
        kingControl2(),
        onBoard1(),
        onBoard2(),
        inReserve1(),
        inReserve2()
    {
        std::fill(onBoard1.begin(), onBoard1.end(), 0);
        std::fill(onBoard2.begin(), onBoard2.end(), 0);
        constexpr unsigned int rows = GameConfig<config>::rows;
        constexpr unsigned int cols = GameConfig<config>::cols;
        size_t pos = 0;
        for(Piece piece : state.board) {
            if(piece.color() == Color::P1) {
                occupied1[pos] = 1;
                ++onBoard1[(int)piece.type()];
                for(Pos npos : GameLogic<rows, cols>::moveSet(piece, pos)) {
                    controlled1[npos.idx()] = 1;
                }
                if(piece.type() == PieceType::King) {
                    kingPosition1[pos] = 1;
                    for(Pos npos : GameLogic<rows, cols>::moveSet(piece, pos)) {
                        kingControl1[npos.idx()] = 1;
                    }
                }
            }
            if(piece.color() == Color::P2) {
                occupied2[pos] = 1;
                ++onBoard2[(int)piece.type()];
                for(Pos npos : GameLogic<rows, cols>::moveSet(piece, pos)) {
                    controlled2[npos.idx()] = 1;
                }
                if(piece.type() == PieceType::King) {
                    kingPosition2[pos] = 1;
                    for(Pos npos : GameLogic<rows, cols>::moveSet(piece, pos)) {
                        kingControl2[npos.idx()] = 1;
                    }
                }
            }
            ++pos;
        }
        std::fill(inReserve1.begin(), inReserve1.end(), 0);
        for(Piece piece : state.reserve1) ++inReserve1[(int)piece.type()];
        std::fill(inReserve2.begin(), inReserve2.end(), 0);
        for(Piece piece : state.reserve2) ++inReserve2[(int)piece.type()];
    }

    size_t nbOccupied1() const { return occupied1.count(); }
    size_t nbOccupied2() const { return occupied2.count(); }

    size_t nbControlled1() const { return controlled1.count(); }
    size_t nbControlled2() const { return controlled2.count(); }

    size_t nbDisputed() const { return (controlled1&controlled2).count(); }

    size_t nbDanger1() const { return (occupied1&controlled2).count(); }
    size_t nbDanger2() const { return (occupied2&controlled1).count(); }

    bool isKingAttacked1() const { return (controlled2&kingPosition1).any(); }
    bool isKingAttacked2() const { return (controlled1&kingPosition2).any(); }

    bool hasKing1() const { return kingPosition1.any(); }
    bool hasKing2() const { return kingPosition2.any(); }

    size_t nbKingEscapes1() const { 
        mask invalid = occupied1 | controlled2;
        return (kingControl1&(~invalid)).count();
    }

    size_t nbKingEscapes2() const { 
        mask invalid = occupied2 | controlled1;
        return (kingControl2&(~invalid)).count();
    }

    size_t kingDistance1() const {
        for(size_t i = 0; i < 12; ++i) {
            if(kingPosition1[i]) return i/3;
        }
        return 0;
    }

    size_t kingDistance2() const {
        for(size_t i = 0; i < 12; ++i) {
            if(kingPosition2[i]) return 3-i/3;
        }
        return 0;
    }

};



#endif