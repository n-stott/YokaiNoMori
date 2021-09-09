#ifndef PIECE_H
#define PIECE_H

#include <string_view>
#include <cassert>

static constexpr const char* PieceCode = "KkPpBbRrQq.";

enum class Piece : char {
    K,
    k,
    P,
    p,
    B,
    b,
    R,
    r,
    Q,
    q,
    E,
};

static bool operator<(Piece a, Piece b) {
    return static_cast<char>(a) < static_cast<char>(b);
}

constexpr Piece pieceOf(const char c) {
    constexpr std::string_view code(PieceCode);
    std::size_t it = code.find(c);
    if(it != std::string_view::npos) {
        return static_cast<Piece>(it);
    }
    assert(false);
    return Piece::E;
}

constexpr char charOf(Piece p) {
    return PieceCode[static_cast<char>(p)];
}

constexpr bool isKing(Piece p)    { return p == Piece::K || p == Piece::k; }
constexpr bool isPawn(Piece p)    { return p == Piece::P || p == Piece::p; }
constexpr bool isBishop(Piece p)  { return p == Piece::B || p == Piece::b; }
constexpr bool isRook(Piece p)    { return p == Piece::R || p == Piece::r; }
constexpr bool isQueen(Piece p)   { return p == Piece::Q || p == Piece::q; }
constexpr bool isEmpty(Piece p)   { return p == Piece::E; }
constexpr bool isPlayer0(Piece p) { return p == Piece::K || p == Piece::P || p == Piece::B || p == Piece::R || p == Piece::Q; }
constexpr bool isPlayer1(Piece p) { return p == Piece::k || p == Piece::p || p == Piece::b || p == Piece::r || p == Piece::q; }
constexpr Piece swapColor(Piece p) { char c = static_cast<char>(p); return isEmpty(p) ? p : static_cast<Piece>(c&1 ? c-1 : c+1); }

#endif