#ifndef ACTION_H
#define ACTION_H

#include "enums.h"
#include "pos.h"
#include <string>
#include "piece.h"

struct Action {
    Piece p;
    ActionType type;
    Pos src;
    Pos dst;
    uint8_t reservePos;

    Action() :
        p(),
        type(ActionType::Drop),
        src(),
        dst(),
        reservePos(-1)
    { }

    Action( Piece p,
            ActionType type,
            Pos src,
            Pos dst,
            uint8_t reservePos) :
        p(p),
        type(type),
        src(src),
        dst(dst),
        reservePos(reservePos)
    { }

    std::string toString() const {
        std::string message;
        message += "Player " + std::to_string(p.color());
        message += " ";
        if(type == Move) {
            message += "moves " + (std::string()+(char)p.toChar()) + " from " + src.toString() + " to " + dst.toString();
        } else {
            message += "drops " + (std::string()+(char)p.toChar()) + " from position " + std::to_string((int)reservePos) + " on " + dst.toString();
        }
        return message;
    }
};

struct ActionSet {
    std::vector<Action> actions;

    std::string toString() const {
        std::string message;
        for(const Action& action : actions) message += action.toString()+'\n';
        return message;
    }

    void push_back(const Action& action) {
        actions.push_back(action);
    }

    bool empty() const {
        return actions.empty();
    }

    void reserve(size_t size) {
        actions.reserve(size);
    }

    using const_iterator = std::vector<Action>::const_iterator;

    const_iterator begin() const { return actions.begin(); }
    const_iterator end() const { return actions.end(); }
};

#endif