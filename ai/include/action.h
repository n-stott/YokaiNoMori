#ifndef ACTION_H
#define ACTION_H

#include "staticvector.h"
#include "enums.h"
#include "pos.h"
#include "piece.h"
#include <string>
#include <utility>
#include <fmt/core.h>

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
        if(type == Move) {
            return fmt::format("Player {} moves {} from {} to {}",
            'A'+(unsigned char)p.color(),
            (char)p.toChar(),
            src.toString(),
            dst.toString()
            );
        } else {
            return fmt::format("Player {} drops {} from position {} on {}",
            'A'+(unsigned char)p.color(),
            (char)p.toChar(),
            (int)src.pos,
            dst.toString()
            );
        }
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
