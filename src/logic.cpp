#include "state.h"
#include "node.h"
#include "tree.h"
#include <iostream>

void test1() {
    State s;
    std::cout << s.niceToString() << '\n';
    auto aa1 = s.allowedActions();
    std::cout << aa1.toString() << '\n';
    s.move(Pawn, P1, 0xB2, 0xB3);
    std::cout << s.niceToString() << '\n';
    auto aa2 = s.allowedActions();
    std::cout << aa2.toString() << '\n';
    s.move(King, P2, 0xB4, 0xB3);
    std::cout << s.niceToString() << '\n';
    auto aa3 = s.allowedActions();
    std::cout << aa3.toString() << '\n';
}

void test2() {
    Tree tree;
    Node t0(&tree);
    std::cout << t0.value.niceToString() << std::endl;
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
    test1();
    test2();
}