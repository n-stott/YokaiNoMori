#include "gamestate.h"
#include "gamelogic.h"
#include <algorithm>
#include <bitset>

template<BoardConfig config>
bool GameState<config>::checkMove(Piece p, Pos a, Pos b) const {
    if(nbTurns >= maxTurns) return false;
    if(hasWon(P1) || hasWon(P2)) return false;
    if(a.valid<config>() == false) return false;
    if(b.valid<config>() == false) return false;
    const Piece src = board.get(a.idx());
    if(src.empty()) return false;
    const Color c = p.color();
    const PieceType pt = p.type();
    if(c != P1 && c != P2) return false;
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

template<BoardConfig config>
bool GameState<config>::checkDrop(Piece p, Pos res, Pos a) const {
    if(nbTurns >= maxTurns) return false;
    if(hasWinner()) return false;
    if(a.valid<config>() == false) return false;
    const Piece dst = board.get(a.idx());
    if(dst.empty() == false) return false;
    const Color c = p.color();
    const PieceType pt = p.type();
    if(pt == NoType) return false;
    if(c != P1 && c != P2) return false;
    if(c != currentPlayer) return false;
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


template<BoardConfig config>
bool GameState<config>::allowedMove(Piece p, Pos a, Pos b) const {
    // Checks that the move is not clearly illegal
    assert(a.valid<config>());
    assert(b.valid<config>());
    const Color c = p.color();
    const PieceType pt = p.type();
    assert(c == P1 || c == P2);
    assert(pt != NoType);
    
    // Checks that are expensive in a loop and could be done before
    assert(!hasWon(P1));
    assert(!hasWon(P2));
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

template<BoardConfig config>
bool GameState<config>::allowedOffset(Piece p, Pos a, Pos b) {
    assert(a.valid<config>());
    assert(b.valid<config>());
    const Color c = p.color();
    const PieceType pt = p.type();
    assert(c == P1 || c == P2);
    assert(pt != NoType);
    auto& movesets = GameLogic<config>::moveSets(p);
    auto& moveset = movesets[a.idx()];
    return std::find(moveset.begin(), moveset.end(), b.idx()) != moveset.end();

}

template<BoardConfig config>
bool GameState<config>::move(Piece p, Pos a, Pos b) {
    assert(allowedMove(p, a, b));
    assert(allowedOffset(p, a, b));
    Piece src = board.get(a.idx()); 
    Piece dst = board.get(b.idx());
    Color c = p.color();
    if(dst.empty() == false) {
        if(c == P1) {
            dst.demote();
            reserve1.push(dst);
        } else {
            dst.demote();
            reserve2.push(dst);
        }
    }
    board.set(a.idx(), Piece());
    constexpr unsigned int rows = GameConfig<config>::rows;
    constexpr unsigned int cols = GameConfig<config>::cols;
    if(c == P1 && b.idx()/cols == 0) {
        src.promote();
    }
    if(c == P2 && b.idx()/cols == rows-1) {
        src.promote();
    }
    board.set(b.idx(), src);
    swapPlayer();
    return true;
}

template<BoardConfig config>
bool GameState<config>::allowedDrop(Piece p, uint8_t& posInReserve, Pos a) const {
    assert(a.valid<config>());
    Color c = p.color();
    PieceType pt = p.type();
    assert(c == P1 || c == P2);
    assert(pt != NoType);
    assert(!hasWinner());
    const Piece dst = board.get(a.idx());
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

template<BoardConfig config>
bool GameState<config>::drop(Piece p, Pos res, Pos dst) {
    uint8_t posInReserve = res.idx();
    assert(allowedDrop(p, posInReserve, dst.idx()));
    const Color c = p.color();
    if(c == P1) {
        const Piece src = reserve1.pop(res.idx());
        board.set(dst.idx(), src);
    }
    if(c == P2) {
        const Piece src = reserve2.pop(res.idx());
        board.set(dst.idx(), src);
    }
    swapPlayer();
    return true;
}

template<BoardConfig config>
void GameState<config>::fillAllowedActions(ActionSet<config>* actions) const {
    actions->clear();
    actions->reserve(64);

    if(hasWon(currentPlayer) || hasLost(currentPlayer)) return;
    if(nbTurns >= maxTurns) return;

    constexpr unsigned int rows = GameConfig<config>::rows;
    constexpr unsigned int cols = GameConfig<config>::cols;

    for(Pos::value i = 0; i < rows*cols; ++i) {
        const Piece p = board.get(i);
        const Pos src(i);
        if(p.color() != currentPlayer) continue;
        for(Pos dst : GameLogic<config>::moveSet(p, src)) {
            if(dst.valid<config>() == false) continue;
            if(allowedMove(p, src, dst) == false) continue;
            actions->push_back(Action<config>::move(p, src, dst));
        }
    }
    if(currentPlayer == P1) {
        std::bitset<NB_PIECE_TYPE+1> typeVisited(false);
        uint8_t dummyPos = 0;
        for(uint8_t k = 0; k < reserve1.size; ++k) {
            const Piece p = reserve1[k];
            if(typeVisited[p.type()]) continue;
            typeVisited[p.type()] = 1;
            for(Pos::value i = 0; i < rows*cols; ++i) {
                const Pos dst(i);
                if(dst.valid<config>() == false) continue;
                if(allowedDrop(p, dummyPos, dst) == false) continue;
                actions->push_back(Action<config>::drop(p, dummyPos, dst));
            }
        }
    }
    else {
        std::bitset<NB_PIECE_TYPE+1> typeVisited(false);
        uint8_t dummyPos = 0;
        for(uint8_t k = 0; k < reserve2.size; ++k) {
            const Piece p = reserve2[k];
            if(typeVisited[p.type()]) continue;
            typeVisited[p.type()] = 1;
            for(Pos::value i = 0; i < rows*cols; ++i) {
                const Pos dst(i);
                if(dst.valid<config>() == false) continue;
                if(allowedDrop(p, dummyPos, dst) == false) continue;
                actions->push_back(Action<config>::drop(p, dummyPos, dst));
            }
        }
    }
}

template class GameState<Easy>;
template class GameState<Medium>;