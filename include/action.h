#ifndef ACTION_H
#define ACTION_H

#include "enums.h"
#include "pos.h"
#include <string>

struct Action {
    Color player;
    ActionType type;
    Pos src;
    Pos dst;
    uint8_t reservePos;

    std::string toString() const {
        std::string message;
        message += "Player " + std::to_string(player);
        message += " ";
        message += (type == Move ? "moves" : "drops");
        message += " from " + src.toString() + " to " + dst.toString();
        return message;
    }
};

#endif