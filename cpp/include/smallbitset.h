#ifndef SMALLBITSET_H
#define SMALLBITSET_H

#include <stdlib.h>

template<unsigned int N, typename UInt>
struct SmallBitset {

    static_assert(N < 8*sizeof(UInt) );

    constexpr SmallBitset() : val(0) { }
    constexpr SmallBitset(UInt v) : val(v) { }

    constexpr SmallBitset operator&=(SmallBitset a) { val &= a.val; return *this; }
    constexpr SmallBitset operator|=(SmallBitset a) { val |= a.val; return *this; }
    
    constexpr SmallBitset operator&(SmallBitset a) const { SmallBitset b = *this; b &= a; return b; }
    constexpr SmallBitset operator|(SmallBitset a) const { SmallBitset b = *this; b |= a; return b; }
    constexpr SmallBitset operator~() const { return SmallBitset{~val}; }

    constexpr void set(uint8_t pos) {
        val |= (1 << pos);
    }

    constexpr size_t count() const {
        return  __builtin_popcount(val);
    }

    constexpr bool any() const { return val; }

    constexpr bool operator[](uint8_t pos) const {
        return (val >> pos) & 1;
    }

    UInt val;
};

#endif