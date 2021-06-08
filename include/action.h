#ifndef ACTION_H
#define ACTION_H

#include "staticvector.h"
#include "enums.h"
#include "pos.h"
#include "piece.h"
#include <string>
#include <utility>

template<BoardConfig config>
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
        message += ('A'+(unsigned char)p.color());
        message += " ";
        if(type == Move) {
            message += "moves " + (std::string()+(char)p.toChar()) + " from " + src.toString<config>() + " to " + dst.toString<config>();
        } else {
            message += "drops " + (std::string()+(char)p.toChar()) + " from position " + std::to_string((int)src.pos) + " on " + dst.toString<config>();
        }
        return message;
    }
};

static_assert(sizeof(Action<Easy>) == 4);
static_assert(sizeof(Action<Medium>) == 4);

template<BoardConfig config>
using aspair = std::pair<Action<config>, double>;

template<BoardConfig config>
struct ActionSet {

    static constexpr unsigned int maxSizeEasy = 64;
    static constexpr unsigned int maxSizeMedium = 128;

    using value_type = Action<config>;
    using storage = static_vector<value_type, (config == Easy ? maxSizeEasy : maxSizeMedium)>;

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
