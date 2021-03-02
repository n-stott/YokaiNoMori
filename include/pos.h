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
        s += std::to_string((pos&0x0F));
        return s;
    }
    
    void check() {
        assert(valid());
    }

    static std::array<Pos::type, 12> validPositions;
};


#endif