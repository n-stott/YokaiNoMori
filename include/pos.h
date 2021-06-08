#ifndef POS_H
#define POS_H

#include "gameconfig.h"
#include <string>
#include <algorithm>
#include <cassert>
#include <array>

struct Pos {

    using value = uint8_t;
    value pos;

    Pos() = default;
    constexpr Pos(value p) : pos(p) { }

    constexpr value idx() const {
        return pos;
    }

    template<BoardConfig config>
    constexpr bool valid() const noexcept {
        constexpr unsigned int rows = GameConfig<config>::rows;
        constexpr unsigned int cols = GameConfig<config>::cols;
        return pos >= 0 && pos < rows*cols;
    }

    constexpr bool operator==(const Pos p) const {
        return pos == p.pos;
    }

    template<BoardConfig config>
    std::string toString() const noexcept {
        std::string s;
        s += ('A' + pos%GameConfig<config>::cols);
        s += std::to_string((pos/GameConfig<config>::cols)+1);
        return s;
    }
};

static_assert(sizeof(Pos) == 1);


#endif