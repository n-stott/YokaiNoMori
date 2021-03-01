#include "state.h"
#include <iostream>

static_assert(sizeof(uint8_t) == 1);
static_assert(sizeof(int8_t) == 1);



int main() {
    State s;
    std::cout << s.niceToString() << '\n';
    auto aa1 = s.allowedActions(P1);
    s.move(Pawn, P1, 0xB2, 0xB3);
    std::cout << s.niceToString() << '\n';
    auto aa2 = s.allowedActions(P2);
    s.move(King, P2, 0xB4, 0xB3);
    std::cout << s.niceToString() << '\n';
}