#include "gamestate.h"
#include "node.h"
#include "tree.h"
#include "agent.h"
#include "search.h"
#include <iostream>

void test1() {
    GameState s;
    std::cout << s.niceToString() << '\n';
    auto aa1 = s.allowedActions();
    std::cout << aa1.toString() << '\n';
    s.move(Pawn, P1, 7, 4);
    std::cout << s.niceToString() << '\n';
    auto aa2 = s.allowedActions();
    std::cout << aa2.toString() << '\n';
    s.move(King, P2, 1, 4);
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
    std::cout << t3.value.niceToString() << '\n';
}

void test3() {
    Tree tree;
    Agent agent(Color::P1);
    Node t0(&tree);
    std::cout << t0.value.niceToString() << std::endl;
    Agent::score s0 = agent.evaluate(t0.value);
    std::cout << s0.s1 << " " << s0.s2 << std::endl;
    t0.computeChildren();
    Node& t1 = t0.children.front();
    std::cout << t1.value.niceToString() << std::endl;
    Agent::score s1 = agent.evaluate(t1.value);
    std::cout << s1.s1 << " " << s1.s2 << std::endl;
    t1.computeChildren();
    Node& t2 = t1.children.front();
    std::cout << t2.value.niceToString() << std::endl;
    Agent::score s2 = agent.evaluate(t2.value);
    std::cout << s2.s1 << " " << s2.s2 << std::endl;
    t2.computeChildren();
    Node& t3 = t2.children.front();
    std::cout << t3.value.niceToString() << std::endl;
    Agent::score s3 = agent.evaluate(t3.value);
    std::cout << s3.s1 << " " << s3.s2 << std::endl;
    t3.computeChildren();
}

void test4() {
    Tree tree;
    Agent agent(Color::P1);
    Node t0(&tree);
    Search search(t0.value, agent);
    double value = search.search(6);
    std::cout << value << std::endl;
    std::cout << "evals : " << agent.nbEvals << std::endl;
}

// int main() {
//     test1();
//     test2();
//     test3();
//     test4();
// }