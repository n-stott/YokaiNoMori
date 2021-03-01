#ifndef POS_H
#define POS_H

#include <string>
#include <algorithm>
#include <cassert>
#include <array>

struct Pos {

    using type = int16_t;

    Pos(type p) : pos(p) { }
    type pos;

    type idx() const {
        return ((pos >> 4)-0xA) + 3*(4- (pos&0x0F));
    }

    static constexpr type h = 0x10;
    static constexpr type v = 0x01;

    bool valid() const {
        return std::find(validPositions.begin(), validPositions.end(), pos) != validPositions.end();
    }

    std::string toString() const noexcept {
        std::string s;
        s += ('A' + ((pos >> 4)-0xA));
        s += std::to_string((pos&0x0F)-1);
        return s;
    }
    
    void check() {
        assert(valid());
    }

    static std::array<Pos::type, 12> validPositions;
};

std::array<Pos::type, 12> Pos::validPositions {
    0xA4, 0xB4, 0xC4,
    0xA3, 0xB3, 0xC3,
    0xA2, 0xB2, 0xC2,
    0xA1, 0xB1, 0xC1
};

#endif