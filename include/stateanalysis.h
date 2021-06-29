#ifndef BOARDANALYSIS_H
#define BOARDANALYSIS_H

#include "gamestate.h"
#include "gamelogic.h"
#include "gameconfig.h"
#include <bitset>
#include <cassert>
#include <array>



template<typename T, unsigned int N, unsigned int... Ns>
struct multi_array {
    std::array< multi_array<T, Ns...>, N> array;

    constexpr multi_array<T, Ns...>& operator[](uint8_t pos) { return array[pos]; }
    constexpr const multi_array<T, Ns...>& operator[](uint8_t pos) const { return array[pos]; }
};

template<typename T, unsigned int N>
struct multi_array<T, N> {
    std::array<T, N> array;

    constexpr T& operator[](uint8_t pos) { return array[pos]; }
    constexpr const T& operator[](uint8_t pos) const { return array[pos]; }
};


template<unsigned int rows, unsigned int cols>
using mask_storage = multi_array<std::bitset<rows*cols>, 2, NB_PIECE_TYPE, rows*cols>;

template<BoardConfig config, unsigned int rows, unsigned int cols>
constexpr mask_storage<rows, cols> computeAllMasks() {
    using mask = std::bitset<rows*cols>;

    mask_storage<rows, cols> result;
    for(uint8_t player = 0; player < 2; ++player) {
        for(uint8_t id = 0; id < NB_PIECE_TYPE; ++id) {
            for(uint8_t pos = 0; pos < rows*cols; ++pos) {
                Piece piece((PieceType)id, (Color)player);
                Pos current(pos);
                unsigned long mask_proxy = 0;
                for(Pos p : GameLogic<config>::moveSet(piece, current)) mask_proxy |= (1 << p.idx());
                mask m(mask_proxy);
                result[player][id][pos] = m;
            }
        }
    }
    return result;
}

template<BoardConfig config>
struct StateAnalysis {

    static constexpr unsigned int rows = GameConfig<config>::rows;
    static constexpr unsigned int cols = GameConfig<config>::cols;
    static constexpr unsigned int ressize = GameConfig<config>::ressize;

    using mask = std::bitset<rows*cols>;

    static constexpr mask_storage<rows, cols> allMasks = computeAllMasks<config, rows, cols>();

    mask occupied1;
    mask occupied2;

    mask controlled1;
    mask controlled2;

    mask kingPosition1;
    mask kingPosition2;

    mask kingControl1;
    mask kingControl2;

    std::array<short, NB_PIECE_TYPE> onBoard1;
    std::array<short, NB_PIECE_TYPE> onBoard2;

    std::array<short, NB_PIECE_TYPE> inReserve1;
    std::array<short, NB_PIECE_TYPE> inReserve2;

    StateAnalysis() { }

    StateAnalysis(const Board<rows, cols>& board, const Reserve<P1, ressize>& reserve1, const Reserve<P2, ressize>& reserve2) :
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
        size_t pos = 0;
        for(Piece piece : board) {
            if(piece.color() == Color::P1) {
                occupied1[pos] = 1;
                ++onBoard1[(int)piece.type()];
                controlled1 |= allMasks[Color::P1][piece.type()][pos];
                if(piece.type() == PieceType::King) {
                    kingPosition1[pos] = 1;
                    kingControl1 = allMasks[Color::P1][PieceType::King][pos];
                }
            }
            if(piece.color() == Color::P2) {
                occupied2[pos] = 1;
                ++onBoard2[(int)piece.type()];
                controlled2 |= allMasks[Color::P2][piece.type()][pos];
                if(piece.type() == PieceType::King) {
                    kingPosition2[pos] = 1;
                    kingControl2 = allMasks[Color::P2][PieceType::King][pos];
                }
            }
            ++pos;
        }
        std::fill(inReserve1.begin(), inReserve1.end(), 0);
        for(Piece piece : reserve1) ++inReserve1[(int)piece.type()];
        std::fill(inReserve2.begin(), inReserve2.end(), 0);
        for(Piece piece : reserve2) ++inReserve2[(int)piece.type()];
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
        for(size_t i = 0; i < rows*cols; ++i) {
            if(kingPosition1[i]) return i/cols;
        }
        return 0;
    }

    size_t kingDistance2() const {
        for(size_t i = 0; i < rows*cols; ++i) {
            if(kingPosition2[i]) return rows-1-i/cols;
        }
        return 0;
    }

};



#endif