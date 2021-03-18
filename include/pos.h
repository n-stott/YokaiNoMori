#ifndef POS_H
#define POS_H

#include <string>
#include <algorithm>
#include <cassert>
#include <array>

struct Pos {

    using value = int16_t;
    value pos;

    constexpr Pos() : pos(-1) { }
    constexpr Pos(value p) : pos(p) { }

    constexpr value idx() const {
        return pos;
    }

    constexpr bool valid() const noexcept {
        return pos >= 0 && pos < 12;
    }

    constexpr bool operator==(const Pos p) const {
        return pos == p.pos;
    }

    std::string toString() const noexcept {
        std::string s;
        s += ('A' + pos%3);
        s += std::to_string((pos/3)+1);
        return s;
    }
};


#endif