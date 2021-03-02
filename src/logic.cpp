#include "state.h"
#include "node.h"
#include <iostream>

void test1() {
    State s;
    std::cout << s.niceToString() << '\n';
    auto aa1 = s.allowedActions();
    s.move(Pawn, P1, 0xB2, 0xB3);
    std::cout << s.niceToString() << '\n';
    auto aa2 = s.allowedActions();
    s.move(King, P2, 0xB4, 0xB3);
    std::cout << s.niceToString() << '\n';
    auto aa3 = s.allowedActions();
}

void test2() {
    State s;
    Node t0(s);
    t0.computeChildren();
    Node& t1 = t0.children.front();
    t1.computeChildren();
    Node& t2 = t1.children.front();
    t2.computeChildren();
    Node& t3 = t2.children.front();
    t3.computeChildren();
    Node& t4 = t3.children.front();
    std::cout << t4.value.niceToString() << '\n';
}

int main() {
    // test1();
    test2();
}