#include "gamestate.h"
#include "gamelogic.h"
#include <algorithm>
#include <bitset>

bool GameState::checkMove(Piece p, Pos a, Pos b) const {
    if(nbTurns >= maxTurns) return false;
    if(hasWon(P0) || hasWon(P1)) return false;
    if(a.valid() == false) return false;
    if(b.valid() == false) return false;
    const Piece src = board.get(a.idx());
    if(src.empty()) return false;
    const Color c = p.color();
    const PieceType pt = p.type();
    if(c != P0 && c != P1) return false;
    if(c != currentPlayer) return false;
    if(pt == NoType) return false;
    if(src.type() != pt) return false;
    if(src.color() != c) return false;
    const Piece dst = board.get(b.idx());
    if(dst.empty() == false) {
        if(dst.color() == c) return false;
    }
    return allowedOffset(p, a, b);
}

bool GameState::checkDrop(Piece p, Pos res, Pos a) const {
    if(nbTurns >= maxTurns) return false;
    if(hasWinner()) return false;
    if(a.valid() == false) return false;
    const Piece dst = board.get(a.idx());
    if(dst.empty() == false) return false;
    const Color c = p.color();
    const PieceType pt = p.type();
    if(pt == NoType) return false;
    if(c != P0 && c != P1) return false;
    if(c != currentPlayer) return false;
    if(c == P0) {
        auto pos = std::find_if(reserve0.begin(), reserve0.end(), [&](Piece p) { return p.type() == pt; });
        if(pos == reserve0.end()) return false;
        uint8_t posInReserve = std::distance(reserve0.begin(), pos);
        if(posInReserve >= reserve0.size) return false;
        const Piece src = reserve0[posInReserve];
        if(src.type() != pt) return false;
        if(src.color() != c) return false;
        if(src.type() == King || src.type() == Queen) return false;
    }
    if(c == P1) {
        auto pos = std::find_if(reserve1.begin(), reserve1.end(), [&](Piece p) { return p.type() == pt; });
        if(pos == reserve1.end()) return false;
        uint8_t posInReserve = std::distance(reserve1.begin(), pos);
        if(posInReserve >= reserve1.size) return false;
        const Piece src = reserve1[posInReserve];
        if(src.type() != pt) return false;
        if(src.color() != c) return false;
        if(src.type() == King || src.type() == Queen) return false;
    }
    return true;
}


bool GameState::allowedMove(Piece p, Pos a, Pos b) const {
    // Checks that the move is not clearly illegal
    assert(a.valid());
    assert(b.valid());
    const Color c = p.color();
    const PieceType pt = p.type();
    assert(c == P0 || c == P1);
    assert(pt != NoType);
    
    // Checks that are expensive in a loop and could be done before
    assert(!hasWon(P0));
    assert(!hasWon(P1));
    assert(nbTurns < maxTurns);

    const Piece src = board.get(a.idx());

    // Check board consistency
    assert(!src.empty());
    assert(src.type() == pt);
    assert(src.color() == c);

    const Piece dst = board.get(b.idx());
    if(dst.empty() == false) {
        if(dst.color() == c) return false;
    }
    return true;
}

bool GameState::allowedOffset(Piece p, Pos a, Pos b) {
    assert(a.valid());
    assert(b.valid());
    const Color c = p.color();
    const PieceType pt = p.type();
    assert(c == P0 || c == P1);
    assert(pt != NoType);
    auto& movesets = GameLogic::moveSets(p);
    auto& moveset = movesets[a.idx()];
    return std::find(moveset.begin(), moveset.end(), b.idx()) != moveset.end();

}

bool GameState::move(Piece p, Pos a, Pos b) {
    assert(allowedMove(p, a, b));
    assert(allowedOffset(p, a, b));
    Piece src = board.get(a.idx()); 
    Piece dst = board.get(b.idx());
    Color c = p.color();
    if(dst.empty() == false) {
        if(c == P0) {
            dst.demote();
            reserve0.push(dst);
        } else {
            dst.demote();
            reserve1.push(dst);
        }
    }
    board.set(a.idx(), Piece());
    constexpr unsigned int rows = GameConfig::rows;
    constexpr unsigned int cols = GameConfig::cols;
    if(c == P0 && b.idx()/cols == 0) {
        src.promote();
    }
    if(c == P1 && b.idx()/cols == rows-1) {
        src.promote();
    }
    board.set(b.idx(), src);
    swapPlayer();
    return true;
}

bool GameState::allowedDrop(Piece p, uint8_t& posInReserve, Pos a) const {
    assert(a.valid());
    Color c = p.color();
    PieceType pt = p.type();
    assert(c == P0 || c == P1);
    assert(pt != NoType);
    assert(!hasWinner());
    const Piece dst = board.get(a.idx());
    if(dst.empty() == false) return false;
    if(c == P0) {
        auto pos = std::find_if(reserve0.begin(), reserve0.end(), [&](Piece p) { return p.type() == pt; });
        if(pos == reserve0.end()) return false;
        posInReserve = std::distance(reserve0.begin(), pos);
#ifndef NDEBUG
        const Piece src = reserve0[posInReserve];
        assert(src.type() == pt);
        assert(src.color() == c);
        assert(src.type() != King);
        assert(src.type() != Queen);
#endif
    } else {
        auto pos = std::find_if(reserve1.begin(), reserve1.end(), [&](Piece p) { return p.type() == pt; });
        if(pos == reserve1.end()) return false;
        posInReserve = std::distance(reserve1.begin(), pos);
#ifndef NDEBUG
        const Piece src = reserve1[posInReserve];
        assert(src.type() == pt);
        assert(src.color() == c);
        assert(src.type() != King);
        assert(src.type() != Queen);
#endif
    }
    return true;
}

bool GameState::drop(Piece p, Pos res, Pos dst) {
    uint8_t posInReserve = res.idx();
    assert(allowedDrop(p, posInReserve, dst.idx()));
    const Color c = p.color();
    if(c == P0) {
        const Piece src = reserve0.pop(res.idx());
        board.set(dst.idx(), src);
    }
    if(c == P1) {
        const Piece src = reserve1.pop(res.idx());
        board.set(dst.idx(), src);
    }
    swapPlayer();
    return true;
}

void GameState::fillAllowedActions(ActionSet* actions) const {
    actions->clear();
    actions->reserve(64);

    if(hasWon(currentPlayer) || hasLost(currentPlayer)) return;
    if(nbTurns >= maxTurns) return;

    constexpr unsigned int rows = GameConfig::rows;
    constexpr unsigned int cols = GameConfig::cols;

    for(Pos::value i = 0; i < rows*cols; ++i) {
        const Piece p = board.get(i);
        const Pos src(i);
        if(p.color() != currentPlayer) continue;
        for(Pos dst : GameLogic::moveSet(p, src)) {
            if(dst.valid() == false) continue;
            if(allowedMove(p, src, dst) == false) continue;
            actions->push_back(Action::move(p, src, dst));
        }
    }
    if(currentPlayer == P0) {
        std::bitset<NB_PIECE_TYPE+1> typeVisited(false);
        uint8_t dummyPos = 0;
        for(uint8_t k = 0; k < reserve0.size; ++k) {
            const Piece p = reserve0[k];
            if(typeVisited[p.type()]) continue;
            typeVisited[p.type()] = 1;
            for(Pos::value i = 0; i < rows*cols; ++i) {
                const Pos dst(i);
                if(dst.valid() == false) continue;
                if(allowedDrop(p, dummyPos, dst) == false) continue;
                actions->push_back(Action::drop(p, dummyPos, dst));
            }
        }
    }
    else {
        std::bitset<NB_PIECE_TYPE+1> typeVisited(false);
        uint8_t dummyPos = 0;
        for(uint8_t k = 0; k < reserve1.size; ++k) {
            const Piece p = reserve1[k];
            if(typeVisited[p.type()]) continue;
            typeVisited[p.type()] = 1;
            for(Pos::value i = 0; i < rows*cols; ++i) {
                const Pos dst(i);
                if(dst.valid() == false) continue;
                if(allowedDrop(p, dummyPos, dst) == false) continue;
                actions->push_back(Action::drop(p, dummyPos, dst));
            }
        }
    }
}