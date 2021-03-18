#include "state.h"

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
        if(i%3 == 0) s+='\n' + std::to_string(4-i/3) + ' ';
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

    for(Pos::type i = 0; i < 12; ++i) {
        const Piece p = board[i];
        const Pos src = Pos::validPositions[i];
        if(p.color() != currentPlayer) continue;
        for(Pos::type offset : p.offsets()) {
            const Pos dst = src.pos + offset;
            if(dst.valid() == false) continue;
            if(allowedMove(p.type(), p.color(), src, dst) == false) continue;
            Action action{p, Move, src, dst, 0};
            actions.push_back(action);
        }
    }
    if(currentPlayer == P1) {
        for(uint8_t k = 0; k < reserve1.size; ++k) {
            const Piece p = reserve1[k];
            for(Pos::type i = 0; i < 12; ++i) {
                const Pos dst = Pos::validPositions[i];
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
            for(Pos::type i = 0; i < 12; ++i) {
                const Pos dst = Pos::validPositions[i];
                if(dst.valid() == false) continue;
                if(allowedDrop(p.type(), p.color(), k, dst) == false) continue;
                Action action{p, Drop, dst, dst, k};
                actions.push_back(action);
            }
        }
    }
    return actions;
}