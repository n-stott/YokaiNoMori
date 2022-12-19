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

    static Action drop(Piece p, uint8_t posInReserve, Pos dst) {
        return Action{p, ActionType::Drop, Pos{posInReserve}, dst};
    }


    std::string toString() const {
        std::string message;
        message += "Player ";
        message += ('A'+(unsigned char)p.color());
        message += " ";
        if(type == Move) {
            message += "moves " + (std::string()+(char)p.toChar()) + " from " + src.toString() + " to " + dst.toString();
        } else {
            message += "drops " + (std::string()+(char)p.toChar()) + " from position " + std::to_string((int)src.pos) + " on " + dst.toString();
        }
        return message;
    }
};

using aspair = std::pair<Action, int>;

struct ActionSet {

    using value_type = Action;
    using storage = static_vector<value_type, 64>;

    storage actions;

    std::string toString() const {
        std::string message;
        for(const value_type& action : actions) message += action.toString()+'\n';
        return message;
    }

    size_t size() const { return actions.size(); }

    void push_back(const value_type& action) {
        if(actions.size() < actions.capacity) actions.push_back(action);
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

    const value_type& operator[](size_t i) const { return actions[i]; }
    value_type& operator[](size_t i) { return actions[i]; }

    using const_iterator = typename storage::const_iterator;

    const_iterator begin() const { return actions.begin(); }
    const_iterator end() const { return actions.end(); }
};


#endif
