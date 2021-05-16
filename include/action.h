#ifndef ACTION_H
#define ACTION_H

#include "staticvector.h"
#include "enums.h"
#include "pos.h"
#include "piece.h"
#include <string>
#include <utility>

struct Action {
    Piece p;
    ActionType type;
    Pos src;
    Pos dst;

    static Action move(Piece p, Pos src, Pos dst) {
        return Action{p, ActionType::Move, src, dst};
    }

    // static Action drop(Piece p, Pos dst) {
    //     return Action{p, ActionType::Drop, Pos{0}, dst};
    // }

    static Action drop(Piece p, uint8_t posInReserve, Pos dst) {
        return Action{p, ActionType::Drop, Pos{posInReserve}, dst};
    }

    std::string toString() const {
        std::string message;
        message += "Player ";
        message += ('A'+(unsigned char)p.color()-1);
        message += " ";
        if(type == Move) {
            message += "moves " + (std::string()+(char)p.toChar()) + " from " + src.toString() + " to " + dst.toString();
        } else {
            message += "drops " + (std::string()+(char)p.toChar()) + " from position " + std::to_string((int)src.pos) + " on " + dst.toString();
        }
        return message;
    }
};

static_assert(sizeof(Action) == 4);

using aspair = std::pair<Action, double>;

struct ActionSet {
    static_vector<Action, 64> actions;

    std::string toString() const {
        std::string message;
        for(const Action& action : actions) message += action.toString()+'\n';
        return message;
    }

    size_t size() const { return actions.size(); }

    void push_back(const Action& action) {
        actions.push_back(action);
    }

    bool empty() const {
        return actions.empty();
    }

    void clear() {
        actions.clear();
    }

    void reserve(size_t size) {
        actions.reserve(size);
    }

    const Action& operator[](size_t i) const { return actions[i]; }
    Action& operator[](size_t i) { return actions[i]; }

    using const_iterator = static_vector<Action, 64>::const_iterator;

    const_iterator begin() const { return actions.begin(); }
    const_iterator end() const { return actions.end(); }
};

#endif