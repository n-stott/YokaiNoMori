#include "state.h"
#include "allowedmove.h"

std::string State::toString() const {
    std::string s;
    for(Piece p : board) s+=p.toChar();
    s+='|';
    s += reserve1.toString();
    s+='|';
    s += reserve2.toString();
    return s;
}

std::string State::niceToString() const {
    std::string s;
    s += "-----------------\n";
    s+="Player B | ";
    s += reserve2.toString();
    s+='\n';
    s += "-----------------";
    for(short i = 0; i < 12; ++i) {
        if(i%3 == 0) s+='\n' + std::to_string(1+i/3) + ' ';
        s += board[i].toChar();
    }
    s += '\n';
    s += "  ABC\n";
    s += "-----------------\n";
    s+="Player a | ";
    s += reserve1.toString();
    s += '\n';
    s += "-----------------";
    return s;
}

bool State::allowedMove(PieceType pt, Color c, Pos a, Pos b) const {
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

bool State::allowedOffset(PieceType pt, Color c, Pos a, Pos b) {
    if(a.valid() == false) return false;
    if(b.valid() == false) return false;
    if(pt == NoType) return false;
    if(c != P1 && c != P2) return false;
    auto& movesets = AllowedMove::get(c, pt);
    auto& moveset = movesets[a.idx()];
    return std::find(moveset.begin(), moveset.end(), b.idx()) != moveset.end();

}

void State::move(PieceType pt, Color c, Pos a, Pos b) {
    assert(allowedMove(pt, c, a, b));
    assert(allowedOffset(pt, c, a, b));
    const Piece src = board[a.idx()]; 
    const Piece dst = board[b.idx()];
    if(dst.empty() == false) {
        if(c == P1) {
            reserve1.push(dst);
        } else {
            reserve2.push(dst);
        }
    }
    board[a.idx()] = Piece();
    board[b.idx()] = src;
    swapPlayer();
}

bool State::allowedDrop(PieceType pt, Color c, uint8_t posInReserve, Pos a) const {
    if(a.valid() == false) return false;
    const Piece dst = board[a.idx()];
    if(dst.empty() == false) return false;
    if(pt == NoType) return false;
    if(c != P1 && c != P2) return false;
    if(c == P1) {
        if(posInReserve >= reserve1.size) return false;
        const Piece src = reserve1[posInReserve];
        if(src.type() != pt) return false;
        if(src.color() != c) return false;
        if(src.type() == King || src.type() == SuperPawn) return false;
    }
    if(c == P2) {
        if(posInReserve >= reserve2.size) return false;
        const Piece src = reserve2[posInReserve];
        if(src.type() != pt) return false;
        if(src.color() != c) return false;
        if(src.type() == King || src.type() == SuperPawn) return false;
    }
    return true;
}

void State::drop(PieceType pt, Color c, uint8_t posInReserve, Pos a) {
    assert(allowedDrop(pt, c, posInReserve, a));
    if(c == P1) {
        const Piece src = reserve1.pop(posInReserve);
        board[a.idx()] = src;
    }
    if(c == P2) {
        const Piece src = reserve2.pop(posInReserve);
        board[a.idx()] = src;
    }
    swapPlayer();
}

ActionSet State::allowedActions() const {
    ActionSet actions;

    for(Pos::value i = 0; i < 12; ++i) {
        const Piece p = board[i];
        const Pos src(i);
        if(p.color() != currentPlayer) continue;
        for(Pos dst : p.moveSet(src)) {
            if(dst.valid() == false) continue;
            if(allowedMove(p.type(), p.color(), src, dst) == false) continue;
            Action action{p, Move, src, dst, 0};
            actions.push_back(action);
        }
    }
    if(currentPlayer == P1) {
        for(uint8_t k = 0; k < reserve1.size; ++k) {
            const Piece p = reserve1[k];
            for(Pos::value i = 0; i < 12; ++i) {
                const Pos dst(i);
                if(dst.valid() == false) continue;
                if(allowedDrop(p.type(), p.color(), k, dst) == false) continue;
                Action action{p, Drop, dst, dst, k};
                actions.push_back(action);
            }
        }
    }
    else {
        for(uint8_t k = 0; k < reserve2.size; ++k) {
            const Piece p = reserve2[k];
            for(Pos::value i = 0; i < 12; ++i) {
                const Pos dst(i);
                if(dst.valid() == false) continue;
                if(allowedDrop(p.type(), p.color(), k, dst) == false) continue;
                Action action{p, Drop, dst, dst, k};
                actions.push_back(action);
            }
        }
    }
    return actions;
}