#ifndef BOARDANALYSIS_H
#define BOARDANALYSIS_H

#include "gamestate.h"
#include "gamelogic.h"
#include "gameconfig.h"
#include "smallbitset.h"
#include <bitset>
#include <cassert>
#include <array>

using mask_t = SmallBitset<GameConfig::rows*GameConfig::cols, unsigned int>;

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


template<unsigned int rows = GameConfig::rows, unsigned int cols = GameConfig::cols>
using mask_storage = multi_array<mask_t, 2, NB_PIECE_TYPE, rows*cols>;

template<unsigned int rows = GameConfig::rows, unsigned int cols = GameConfig::cols>
constexpr mask_storage<> computeAllMasks() {
    using mask = mask_t;

    mask_storage<> result;
    for(uint8_t player = 0; player < 2; ++player) {
        for(uint8_t id = 0; id < NB_PIECE_TYPE; ++id) {
            for(uint8_t pos = 0; pos < rows*cols; ++pos) {
                Piece piece((PieceType)id, (Color)player);
                Pos current(pos);
                unsigned long mask_proxy = 0;
                for(Pos p : GameLogic::moveSet(piece, current)) mask_proxy |= (1 << p.idx());
                mask m(mask_proxy);
                result[player][id][pos] = m;
            }
        }
    }
    return result;
}

struct StateAnalysis {

    static constexpr unsigned int rows = GameConfig::rows;
    static constexpr unsigned int cols = GameConfig::cols;
    static constexpr unsigned int ressize = GameConfig::ressize;

    using mask = mask_t;

    static constexpr mask_storage<> allMasks = computeAllMasks();

    mask occupied0;
    mask occupied1;

    mask controlled0;
    mask controlled1;

    mask kingPosition0;
    mask kingPosition1;

    mask kingControl0;
    mask kingControl1;

    std::array<short, NB_PIECE_TYPE> onBoard0;
    std::array<short, NB_PIECE_TYPE> onBoard1;

    std::array<short, NB_PIECE_TYPE> inReserve0;
    std::array<short, NB_PIECE_TYPE> inReserve1;

    StateAnalysis() = default;

    StateAnalysis(const Board& board, const Reserve<P0, ressize>& reserve0, const Reserve<P1, ressize>& reserve1) :
        occupied0(),
        occupied1(),
        controlled0(),
        controlled1(),
        kingPosition0(),
        kingPosition1(),
        kingControl0(),
        kingControl1(),
        onBoard0(),
        onBoard1(),
        inReserve0(),
        inReserve1()
    {
        std::fill(onBoard0.begin(), onBoard0.end(), 0);
        std::fill(onBoard1.begin(), onBoard1.end(), 0);
        size_t pos = 0;
        for(Piece piece : board) {
            if(piece.color() == Color::P0) {
                occupied0.set(pos);
                ++onBoard0[(int)piece.type()];
                controlled0 |= allMasks[Color::P0][piece.type()][pos];
                if(piece.type() == PieceType::King) {
                    kingPosition0.set(pos);
                    kingControl0 = allMasks[Color::P0][PieceType::King][pos];
                }
            }
            if(piece.color() == Color::P1) {
                occupied1.set(pos);
                ++onBoard1[(int)piece.type()];
                controlled1 |= allMasks[Color::P1][piece.type()][pos];
                if(piece.type() == PieceType::King) {
                    kingPosition1.set(pos);
                    kingControl1 = allMasks[Color::P1][PieceType::King][pos];
                }
            }
            ++pos;
        }
        std::fill(inReserve0.begin(), inReserve0.end(), 0);
        for(Piece piece : reserve0) ++inReserve0[(int)piece.type()];
        std::fill(inReserve1.begin(), inReserve1.end(), 0);
        for(Piece piece : reserve1) ++inReserve1[(int)piece.type()];
    }

    size_t nbOccupied0() const { return occupied0.count(); }
    size_t nbOccupied1() const { return occupied1.count(); }

    size_t nbControlled0() const { return controlled0.count(); }
    size_t nbControlled1() const { return controlled1.count(); }

    size_t nbDisputed() const { return (controlled0&controlled1).count(); }

    size_t nbDanger0() const { return (occupied0&controlled1).count(); }
    size_t nbDanger1() const { return (occupied1&controlled0).count(); }

    bool isKingAttacked0() const { return (controlled1&kingPosition0).any(); }
    bool isKingAttacked1() const { return (controlled0&kingPosition1).any(); }

    bool hasKing0() const { return kingPosition0.any(); }
    bool hasKing1() const { return kingPosition1.any(); }

    size_t nbKingEscapes0() const { 
        mask invalid = occupied0 | controlled1;
        return (kingControl0&(~invalid)).count();
    }

    size_t nbKingEscapes1() const { 
        mask invalid = occupied1 | controlled0;
        return (kingControl1&(~invalid)).count();
    }

    size_t kingDistance0() const {
        if(!hasKing0()) return 0;
        return rows-1-__builtin_ctz(kingPosition0.val)/cols;
    }

    size_t kingDistance1() const {
        if(!hasKing1()) return 0;
        return __builtin_ctz(kingPosition1.val)/cols;
    }

};



#endif