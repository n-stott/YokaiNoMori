#ifndef RESERVE_H
#define RESERVE_H

#include "piece.h"
#include <array>
#include <cassert>
#include <string>

template<Color c, unsigned int ressize>
struct Reserve {

    std::array<Piece, ressize> reserve;
    uint8_t size;

    Reserve() :
        reserve(),
        size(0)
    { }

    Reserve(const std::string& res) :
        reserve(),
        size(0)
    {
        if(res.length() == 1 && res[0] == '.') return;
        for(size_t i = 0; i < res.size(); ++i) {
            push(Piece(res[i]));
            if(size == ressize) assert(false);
        }
    }

    void push(const Piece p) {
        assert(size < ressize);
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

    const Piece* begin() const { return reserve.data(); }
    const Piece* end() const { return reserve.data()+size; }

};

#endif