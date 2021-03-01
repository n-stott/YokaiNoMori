#include "staticvector.h"

#include <array>
#include <vector>
#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <algorithm>

static_assert(sizeof(uint8_t) == 1);
static_assert(sizeof(int8_t) == 1);

enum PieceType {
    NoType,
    King,
    Tower,
    Rook,
    Pawn,
    SuperPawn
};

enum Color {
    None,
    P1,
    P2
};

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

class Piece {
private:
    uint8_t data_;

public:
    constexpr Piece() noexcept : data_(0) {}
    constexpr Piece(PieceType pt, Color c) noexcept : data_( pt | (c << 3) ) {}

    constexpr bool empty() const noexcept { return data_ == 0; }
    
    constexpr PieceType type() const noexcept { return (PieceType)(data_ & 0b0000111); }
    constexpr Color color() const noexcept    { return (Color)(data_ >> 3); }
    constexpr void setColor(Color c) noexcept { data_ = (data_ & 0b00000111) | (c << 3); }

    constexpr char toChar() const noexcept {
        char ret = '.';
        if(type() == King) ret = 'k';
        if(type() == Tower) ret = 't';
        if(type() == Rook) ret = 'r';
        if(type() == Pawn) ret = 'p';
        if(type() == SuperPawn) ret = 's';
        if(color() == P2) ret -= 32;
        return ret;
    }

    const static_vector<Pos::type, 8>& offsets() const {
        if(type() == King) return kingOffsets;
        if(type() == Tower) return towerOffsets;
        if(type() == Rook) return rookOffsets;
        if(color() == P1 && type() == Pawn) return p1PawnOffsets;
        if(color() == P1 && type() == SuperPawn) return p1SuperPawnOffsets;
        if(color() == P2 && type() == Pawn) return p2PawnOffsets;
        if(color() == P2 && type() == SuperPawn) return p2SuperPawnOffsets;
        assert(false);
        return kingOffsets;
    }

private:

    static static_vector<Pos::type, 8> kingOffsets;
    static static_vector<Pos::type, 8> towerOffsets;
    static static_vector<Pos::type, 8> rookOffsets;
    static static_vector<Pos::type, 8> p1PawnOffsets;
    static static_vector<Pos::type, 8> p2PawnOffsets;
    static static_vector<Pos::type, 8> p1SuperPawnOffsets;
    static static_vector<Pos::type, 8> p2SuperPawnOffsets;
};

static_vector<Pos::type, 8> Piece::kingOffsets {
    Pos::v, Pos::v+Pos::h, Pos::h, Pos::h-Pos::v, -Pos::v, -Pos::v-Pos::h, -Pos::h, -Pos::h+Pos::v
};

static_vector<Pos::type, 8> Piece::towerOffsets {
    Pos::v, Pos::h, -Pos::v, -Pos::h
};

static_vector<Pos::type, 8> Piece::rookOffsets {
    +Pos::v+Pos::h, +Pos::v-Pos::h, -Pos::v+Pos::h, -Pos::v-Pos::h
};

static_vector<Pos::type, 8> Piece::p1PawnOffsets {
    Pos::v
};

static_vector<Pos::type, 8> Piece::p2PawnOffsets {
    -Pos::v
};

static_vector<Pos::type, 8> Piece::p1SuperPawnOffsets {
    Pos::v, Pos::v+Pos::h, Pos::h, -Pos::v, -Pos::h, -Pos::h+Pos::v
};

static_vector<Pos::type, 8> Piece::p2SuperPawnOffsets {
    Pos::v, Pos::h, Pos::h-Pos::v, -Pos::v, -Pos::v-Pos::h, -Pos::h
};


enum ActionType {
    Move,
    Drop
};

struct Action {
    Color player;
    ActionType type;
    Pos src;
    Pos dst;
    uint8_t reservePos;

    std::string toString() const {
        std::string message;
        message += "Player " + std::to_string(player);
        message += " ";
        message += (type == Move ? "moves" : "drops");
        message += " from " + src.toString() + " to " + dst.toString();
        return message;
    }
};

struct State {
    std::array<Piece, 12> board;
    std::array<Piece, 6> reserve1;
    uint8_t size1;
    std::array<Piece, 6> reserve2;
    uint8_t size2;

    State() : 
        board({
            Piece(Tower, P2), Piece(King, P2), Piece(Rook, P2),
            Piece(),          Piece(Pawn, P2), Piece(),
            Piece(),          Piece(Pawn, P1), Piece(),
            Piece(Rook, P1),  Piece(King, P1), Piece(Tower, P1)
        }),
        reserve1(),
        size1(0),
        reserve2(),
        size2(0)
    { }

    std::string toString() const {
        std::string s;
        for(Piece p : board) s+=p.toChar();
        s+='|';
        for(short r = 0; r < size1; ++r) s+=reserve1[r].toChar();
        s+='|';
        for(short r = 0; r < size2; ++r) s+=reserve2[r].toChar();
        return s;
    }

    std::string niceToString() const {
        std::string s;
        s += "-----------------\n";
        s+="Player B | ";
        for(short r = 0; r < size2; ++r) s+=reserve2[r].toChar();
        s+='\n';
        s += "-----------------";
        for(short i = 0; i < 12; ++i) {
            if(i%3 == 0) s+='\n' + std::to_string(3-i/3) + ' ';
            s += board[i].toChar();
        }
        s += '\n';
        s += "  ABC\n";
        s += "-----------------\n";
        s+="Player a | ";
        for(short r = 0; r < size1; ++r) s+=reserve1[r].toChar();
        s += '\n';
        s += "-----------------";
        return s;
    }

    void pushReserve1(const Piece p) {
        reserve1[size1] = p;
        reserve1[size1].setColor(P1);
        size1++;
    }

    Piece popReserve1(uint8_t pos) {
        assert(pos < size1);
        size1--;
        std::swap(reserve1[pos], reserve1[size1]);
        return reserve1[size1];
    }

    void pushReserve2(const Piece p) {
        reserve2[size2] = p;
        reserve2[size2].setColor(P2);
        size2++;
    }

    Piece popReserve2(uint8_t pos) {
        assert(pos < size2);
        size2--;
        std::swap(reserve2[pos], reserve2[size1]);
        return reserve2[size1];
    }

    bool allowedMove(PieceType pt, Color c, Pos a, Pos b) const {
        if(a.valid() == false) return false;
        if(b.valid() == false) return false;
        const Piece src = board[a.idx()];
        if(src.empty()) return false;
        if(c != P1 && c != P2) return false;
        if(pt == NoType) return false;
        if(src.type() != pt) return false;
        if(src.color() != c) return false;
        const Piece dst = board[b.idx()];
        if(dst.empty() == false) {
            if(dst.color() == c) return false;
        }
        return true;
    }

    static bool allowedOffset(PieceType pt, Color c, Pos a, Pos b) {
        if(a.valid() == false) return false;
        if(b.valid() == false) return false;
        if(pt == NoType) return false;
        if(c != P1 && c != P2) return false;
        const Pos::type roffset = b.pos - a.pos;
        const Pos::type aoffset = std::abs(roffset);
        if(pt == King) {
            return(aoffset == Pos::h
                || aoffset == std::abs(Pos::v-Pos::h)
                || aoffset == Pos::v
                || aoffset == Pos::v+Pos::h);
        }
        if(pt == Tower) {
            return(aoffset == Pos::h
                || aoffset == Pos::v);
        }
        if(pt == Rook) {
            return(aoffset == std::abs(Pos::v-Pos::h)
                || aoffset == Pos::h+Pos::v);
        }
        if(pt == Pawn) {
            if(c == P1) {
                return roffset == Pos::v;
            } else {
                return roffset == -Pos::v;
            }
        }
        if(pt == SuperPawn) {
            if(c == P1) {
                return (aoffset == Pos::v
                     || aoffset == Pos::h
                     || roffset == Pos::v + Pos::h
                     || roffset == Pos::v - Pos::h);
            } else {
                return (aoffset == Pos::v
                     || aoffset == Pos::h
                     || roffset == -Pos::v + Pos::h
                     || roffset == -Pos::v - Pos::h);
            }
        }
        assert(false);
        return false;
    }

    void move(PieceType pt, Color c, Pos a, Pos b) {
        assert(allowedMove(pt, c, a, b));
        assert(allowedOffset(pt, c, a, b));
        const Piece src = board[a.idx()]; 
        const Piece dst = board[b.idx()];
        if(dst.empty() == false) {
            if(c == P1) {
                pushReserve1(dst);
            } else {
                pushReserve2(dst);
            }
        }
        board[a.idx()] = Piece();
        board[b.idx()] = src;
    }

    bool allowedDrop(PieceType pt, Color c, uint8_t posInReserve, Pos a) const {
        if(a.valid() == false) return false;
        const Piece dst = board[a.idx()];
        if(dst.empty() == false) return false;
        if(pt == NoType) return false;
        if(c != P1 && c != P2) return false;
        if(c == P1) {
            if(posInReserve >= size1) return false;
            const Piece src = reserve1[posInReserve];
            if(src.type() != pt) return false;
            if(src.color() != c) return false;
            if(src.type() == King || src.type() == SuperPawn) return false;
        }
        if(c == P2) {
            if(posInReserve >= size2) return false;
            const Piece src = reserve2[posInReserve];
            if(src.type() != pt) return false;
            if(src.color() != c) return false;
            if(src.type() == King || src.type() == SuperPawn) return false;
        }
        return true;
    }

    void drop(PieceType pt, Color c, uint8_t posInReserve, Pos a) {
        assert(allowedDrop(pt, c, posInReserve, a));
        if(c == P1) {
            const Piece src = popReserve1(posInReserve);
            board[a.idx()] = src;
        }
        if(c == P2) {
            const Piece src = popReserve2(posInReserve);
            board[a.idx()] = src;
        }
    }

    std::vector<Action> allowedActions(Color c) const {
        std::vector<Action> actions;

        for(Pos::type i = 0; i < 12; ++i) {
            const Piece p = board[i];
            const Pos src = Pos::validPositions[i];
            if(p.color() != c) continue;
            for(Pos::type offset : p.offsets()) {
                const Pos dst = src.pos + offset;
                if(dst.valid() == false) continue;
                if(allowedMove(p.type(), p.color(), src, dst) == false) continue;
                Action action{c, Move, src, dst, 0};
                std::cerr << action.toString() << '\n';
                actions.push_back(action);
            }
        }
        return actions;
    }

private:

};

int main() {
    State s;
    std::cout << s.niceToString() << '\n';
    auto aa1 = s.allowedActions(P1);
    s.move(Pawn, P1, 0xB2, 0xB3);
    std::cout << s.niceToString() << '\n';
    auto aa2 = s.allowedActions(P2);
    s.move(King, P2, 0xB4, 0xB3);
    std::cout << s.niceToString() << '\n';
}