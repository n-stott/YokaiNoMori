#include "gamestate.h"
#include "allowedmove.h"
#include <algorithm>
#include <bitset>

std::string GameState::toString() const {
    std::string s;
    for(Piece p : board) s+=p.toChar();
    s += '|';
    s += reserve1.toString();
    s += '|';
    s += reserve2.toString();
    return s;
}

std::string GameState::niceToString() const {
    std::string s;
    s += "-----------------\n";
    s += "Player B | ";
    s += reserve2.toString();
    s += '\n';
    s += "-----------------";
    for(short i = 0; i < 12; ++i) {
        if(i%3 == 0) s += '\n' + std::to_string(1+i/3) + ' ';
        s += board[i].toChar();
    }
    s += '\n';
    s += "  ABC\n";
    s += "-----------------\n";
    s += "Player a | ";
    s += reserve1.toString();
    s += '\n';
    s += "-----------------";
    return s;
}

bool GameState::checkMove(PieceType pt, Color c, Pos a, Pos b) const {
    if(nbTurns >= maxTurns) return false;
    if(hasWon(P1) || hasWon(P2)) return false;
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

bool GameState::checkDrop(PieceType pt, Color c, Pos res, Pos a) const {
    if(nbTurns >= maxTurns) return false;
    if(hasWinner()) return false;
    if(a.valid() == false) return false;
    const Piece dst = board[a.idx()];
    if(dst.empty() == false) return false;
    if(pt == NoType) return false;
    if(c != P1 && c != P2) return false;
    if(c == P1) {
        auto pos = std::find_if(reserve1.begin(), reserve1.end(), [&](Piece p) { return p.type() == pt; });
        if(pos == reserve1.end()) return false;
        uint8_t posInReserve = std::distance(reserve1.begin(), pos);
        if(posInReserve >= reserve1.size) return false;
        const Piece src = reserve1[posInReserve];
        if(src.type() != pt) return false;
        if(src.color() != c) return false;
        if(src.type() == King || src.type() == SuperPawn) return false;
    }
    if(c == P2) {
        auto pos = std::find_if(reserve2.begin(), reserve2.end(), [&](Piece p) { return p.type() == pt; });
        if(pos == reserve2.end()) return false;
        uint8_t posInReserve = std::distance(reserve2.begin(), pos);
        if(posInReserve >= reserve2.size) return false;
        const Piece src = reserve2[posInReserve];
        if(src.type() != pt) return false;
        if(src.color() != c) return false;
        if(src.type() == King || src.type() == SuperPawn) return false;
    }
    return true;
}


bool GameState::allowedMove(PieceType pt, Color c, Pos a, Pos b) const {
    // Checks that the move is not clearly illegal
    assert(a.valid());
    assert(b.valid());
    assert(c == P1 || c == P2);
    assert(pt != NoType);
    
    // Checks that are expensive in a loop and could be done before
    assert(!hasWon(P1));
    assert(!hasWon(P2));
    assert(nbTurns < maxTurns);

    const Piece src = board[a.idx()];

    // Check board consistency
    assert(!src.empty());
    assert(src.type() == pt);
    assert(src.color() == c);

    const Piece dst = board[b.idx()];
    if(dst.empty() == false) {
        if(dst.color() == c) return false;
    }
    return true;
}

bool GameState::allowedOffset(PieceType pt, Color c, Pos a, Pos b) {
    assert(a.valid());
    assert(b.valid());
    assert(c == P1 || c == P2);
    assert(pt != NoType);
    auto& movesets = AllowedMove::get(c, pt);
    auto& moveset = movesets[a.idx()];
    return std::find(moveset.begin(), moveset.end(), b.idx()) != moveset.end();

}

bool GameState::move(PieceType pt, Color c, Pos a, Pos b) {
    if(!allowedMove(pt, c, a, b)) return false;
    if(!allowedOffset(pt, c, a, b)) return false;
    const Piece src = board[a.idx()]; 
    Piece dst = board[b.idx()];
    if(dst.empty() == false) {
        if(c == P1) {
            dst.demote();
            reserve1.push(dst);
        } else {
            dst.demote();
            reserve2.push(dst);
        }
    }
    board[a.idx()] = Piece();
    board[b.idx()] = src;
    if(c == P1 && b.idx()/3 == 0) {
        board[b.idx()].promote();
    }
    if(c == P2 && b.idx()/3 == 3) {
        board[b.idx()].promote();
    }
    swapPlayer();
    return true;
}

bool GameState::allowedDrop(PieceType pt, Color c, uint8_t& posInReserve, Pos a) const {
    assert(a.valid());
    assert(c == P1 || c == P2);
    assert(pt != NoType);
    assert(!hasWinner());
    const Piece dst = board[a.idx()];
    if(dst.empty() == false) return false;
    if(c == P1) {
        auto pos = std::find_if(reserve1.begin(), reserve1.end(), [&](Piece p) { return p.type() == pt; });
        if(pos == reserve1.end()) return false;
        posInReserve = std::distance(reserve1.begin(), pos);
#ifndef NDEBUG
        const Piece src = reserve1[posInReserve];
        assert(src.type() == pt);
        assert(src.color() == c);
        assert(src.type() != King);
        assert(src.type() != SuperPawn);
#endif
    } else {
        auto pos = std::find_if(reserve2.begin(), reserve2.end(), [&](Piece p) { return p.type() == pt; });
        if(pos == reserve2.end()) return false;
        posInReserve = std::distance(reserve2.begin(), pos);
#ifndef NDEBUG
        const Piece src = reserve2[posInReserve];
        assert(src.type() == pt);
        assert(src.color() == c);
        assert(src.type() != King);
        assert(src.type() != SuperPawn);
#endif
    }
    return true;
}

bool GameState::drop(PieceType pt, Color c, Pos res, Pos dst) {
    uint8_t posInReserve = res.idx();
    assert(allowedDrop(pt, c, posInReserve, dst.idx()));
    if(c == P1) {
        const Piece src = reserve1.pop(res.idx());
        board[dst.idx()] = src;
    }
    if(c == P2) {
        const Piece src = reserve2.pop(res.idx());
        board[dst.idx()] = src;
    }
    swapPlayer();
    return true;
}


bool GameState::hasWon(Color player) const {
    if(player == Color::P1) {
        return std::any_of(reserve1.begin(), reserve1.end(), [](Piece p){ return p.type() == PieceType::King; });
    } else {
        return std::any_of(reserve2.begin(), reserve2.end(), [](Piece p){ return p.type() == PieceType::King; });
    }
}

bool GameState::hasLost(Color player) const {
    if(player == Color::P1) {
        return hasWon(Color::P2);
    } else {
        return hasWon(Color::P1);
    }
}

void GameState::fillAllowedActions(ActionSet* actions) const {
    actions->clear();
    actions->reserve(64);

    if(hasWon(currentPlayer) || hasLost(currentPlayer)) return;
    if(nbTurns >= maxTurns) return;

    for(Pos::value i = 0; i < 12; ++i) {
        const Piece p = board[i];
        const Pos src(i);
        if(p.color() != currentPlayer) continue;
        for(Pos dst : p.moveSet(src)) {
            if(dst.valid() == false) continue;
            if(allowedMove(p.type(), p.color(), src, dst) == false) continue;
            actions->push_back(Action::move(p, src, dst));
        }
    }
    if(currentPlayer == P1) {
        std::bitset<6> typeVisited(false);
        uint8_t dummyPos = 0;
        for(uint8_t k = 0; k < reserve1.size; ++k) {
            const Piece p = reserve1[k];
            if(typeVisited[p.type()]) continue;
            typeVisited[p.type()] = 1;
            for(Pos::value i = 0; i < 12; ++i) {
                const Pos dst(i);
                if(dst.valid() == false) continue;
                if(allowedDrop(p.type(), p.color(), dummyPos, dst) == false) continue;
                actions->push_back(Action::drop(p, dummyPos, dst));
            }
        }
    }
    else {
        std::bitset<6> typeVisited(false);
        uint8_t dummyPos = 0;
        for(uint8_t k = 0; k < reserve2.size; ++k) {
            const Piece p = reserve2[k];
            if(typeVisited[p.type()]) continue;
            typeVisited[p.type()] = 1;
            for(Pos::value i = 0; i < 12; ++i) {
                const Pos dst(i);
                if(dst.valid() == false) continue;
                if(allowedDrop(p.type(), p.color(), dummyPos, dst) == false) continue;
                actions->push_back(Action::drop(p, dummyPos, dst));
            }
        }
    }
}
