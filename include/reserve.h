#ifndef RESERVE_H
#define RESERVE_H

#include "piece.h"
#include <array>
#include <cassert>
#include <string>

template<Color c>
struct Reserve {

    std::array<Piece, 6> reserve;
    uint8_t size;

    void push(const Piece p) {
        reserve[size] = p;
        reserve[size].setColor(c);
        size++;
    }

    Piece pop(uint8_t pos) {
        assert(pos < size);
        size--;
        std::swap(reserve[pos], reserve[size]);
        return reserve[size];
    }

    std::string toString() const {
        std::string s;
        for(uint8_t i = 0; i < size; ++i) {
            s += reserve[i].toChar();
        }
        return s;
    }

    const Piece& operator[](uint8_t pos) const {
        assert(pos < size);
        return reserve[pos];
    }

};

#endif