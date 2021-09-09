#ifndef NODE_H
#define NODE_H

#include "piece.h"
#include "moves/allowedmove.h"
#include <array>
#include <string_view>
#include <string>
#include <deque>

// TODO : memory footprint can be divided by 2 : Piece fits on a half byte

struct Node {
    using reserve_t = std::array<Piece, 7>;
    using board_t = std::array<Piece, 12>;

    reserve_t res0;
    board_t board;
    reserve_t res1;
    bool player;
    short age;

    Node(const reserve_t& res0, const board_t& board, const reserve_t& res1, const bool& player, short age) :
        res0(res0), board(board), res1(res1), player(player), age(age)
    {

    }
    
    Node(std::string_view fen) {
        const char* it = fen.data();
        size_t pos = 0;
        while(*it != '/') {
            res0[pos] = pieceOf(*it);
            ++pos;
            ++it;
        }
        for(; pos < 7; ++pos) {
            res0[pos] = Piece::E;
        }
        ++it;
        for(int i = 0; i < 12; ++i) {
            board[i] = pieceOf(*it);
            ++it;
        }
        pos = 0;
        while(*it != '/') {
            res1[pos] = pieceOf(*it);
            ++pos;
            ++it;
        }
        for(; pos < 7; ++pos) {
            res1[pos] = Piece::E;
        }
        ++it;
        player = (*it == '1');
        age = 0;
    }

    std::string toString() const {
        std::string s;
        s.reserve(7+1+12+1+7+1+1);
        for(Piece p : res0) s += charOf(p);
        s += ' ';
        for(Piece p : board) s += charOf(p);
        s += ' ';
        for(Piece p : res1) s += charOf(p);
        s += ' ';
        s += std::to_string(player);
        return s;
    }

    bool operator==(const Node& other) const {
        return player == other.player && board == other.board && res0 == other.res0 && res1 == other.res1;
    }

    template<bool not_stupid> // only future is win if opposite king can be eaten
    std::deque<Node> successors() const {
        std::deque<Node> s;
        if(player == 0) {
            for(int i = 0; i < 7; ++i) {
                Piece p = res0[i];
                if(res0[i] != Piece::E) {
                    for(int j = 0; j < 12; ++j) {
                        if(board[j] == Piece::E) {
                            reserve_t newres = res0;
                            newres[i] = Piece::E;
                            std::sort(newres.begin(), newres.end());
                            board_t newboard = board;
                            newboard[j] = p;
                            s.emplace_back(newres, newboard, res1, !player, age+1);
                        }
                    } 
                }
            }
            for(int i = 0; i < 12; ++i) {
                Piece p = board[i];
                if(isPlayer0(p)) {
                    for(uint8_t dst : moveSet(p, i)) {
                        Piece q = board[dst];
                        if(not_stupid && isPlayer1(q) && isKing(q)) {
                            s.clear();
                            return s;
                        }
                        if(isPlayer1(q) && !isKing(q)) {
                            reserve_t newres = res0;
                            assert(isEmpty(res0[6]));
                            newres[6] = swapColor(q);
                            std::sort(newres.begin(), newres.end());
                            board_t newboard = board;
                            newboard[dst] = p;
                            newboard[i] = Piece::E;
                            s.emplace_back(newres, newboard, res1, !player, age+1);
                        }
                        if(isEmpty(q)) {
                            board_t newboard = board;
                            newboard[dst] = p;
                            newboard[i] = Piece::E;
                            s.emplace_back(res0, newboard, res1, !player, age+1);
                        }
                    }
                }
            }

        } else {
            for(int i = 0; i < 7; ++i) {
                Piece p = res1[i];
                if(res1[i] != Piece::E) {
                    for(int j = 0; j < 12; ++j) {
                        if(board[j] == Piece::E) {
                            reserve_t newres = res1;
                            newres[i] = Piece::E;
                            std::sort(newres.begin(), newres.end());
                            board_t newboard = board;
                            newboard[j] = p;
                            s.emplace_back(res0, newboard, newres, !player, age+1);
                        }
                    } 
                }
            }
            for(int i = 0; i < 12; ++i) {
                Piece p = board[i];
                if(isPlayer1(p)) {
                    for(uint8_t dst : moveSet(p, i)) {
                        Piece q = board[dst];
                        if(not_stupid && isPlayer0(q) && isKing(q)) {
                            s.clear();
                            return s;
                        }
                        if(isPlayer0(q) && !isKing(q)) {
                            reserve_t newres = res1;
                            assert(isEmpty(res1[6]));
                            newres[6] = swapColor(q);
                            std::sort(newres.begin(), newres.end());
                            board_t newboard = board;
                            newboard[dst] = p;
                            newboard[i] = Piece::E;
                            s.emplace_back(res0, newboard, newres, !player, age+1);
                        }
                        if(isEmpty(q)) {
                            board_t newboard = board;
                            newboard[dst] = p;
                            newboard[i] = Piece::E;
                            s.emplace_back(res0, newboard, res1, !player, age+1);
                        }
                    }
                }
            }
        }
        return s;
    }


    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v) const {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }

    std::size_t hash() const {
        std::size_t hash = 0;
        for(Piece p : res0) hash_combine(hash, static_cast<char>(p));
        for(Piece p : res1) hash_combine(hash, static_cast<char>(p));
        for(Piece p : board) hash_combine(hash, static_cast<char>(p));
        hash_combine(hash, player);
        return hash;
    }
};

#endif