#include "gamestate.h"
#include "node.h"
#include "tree.h"
#include "agent.h"
#include "minimax.h"
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
    Agent agent;
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
    Agent agent;
    Node t0(&tree);
    Minimax search(t0.value, agent, 7);
    double value = search.run(Minimax::PureMinimax);
    std::cout << value << std::endl;
    std::cout << "evals : " << agent.nbEvals << std::endl;
    std::cout << "Best move : " << search.bestAction.value().toString() << std::endl;
}

void test5() {
    GameState game("kK", "", "", P1);
    std::cout << game.niceToString() << std::endl;

    Agent agent1;
    int depth = 1;
    Minimax search1(game, agent1, depth);
    search1.run(Minimax::PureMinimax);
    std::optional<Action> action = search1.bestAction;
    if(action) {
        std::cout << action.value().toString() << std::endl;
        game.apply(action.value());
    }

    std::cout << game.niceToString() << std::endl;
}

void test6() {
    GameState game("k", "K", "", P2);
    std::cout << game.niceToString() << std::endl;
    std::cout << game.hasWinner() << std::endl;
}

void test7() {
    GameState game("TeeKReePkret", "", "P", P1);
    std::cout << game.niceToString() << std::endl;
}

void test8() {
    GameState game("eerKekeReTee", "stp", "", P2);
    std::cout << game.niceToString() << std::endl;
    Agent agent;
    int depth = 6;
    Minimax::Mode mode = Minimax::PureMinimax; 
    Minimax search(game, agent, depth);
    search.run(mode);
}

// void test8() {
//     GameState game;
//     Agent agent1;
//     Agent agent2;

//     int depth = 6;
//     // Minimax::Mode mode = Minimax::PureMinimax;
//     Minimax::Mode mode = Minimax::AlphaBeta; 

//     while(!game.hasWinner()) {
//         std::cout << game.niceToString() << std::endl;
//         std::cout << "Turn of player : " << (game.currentPlayer == P1 ? "A" : "B") << std::endl;
//         std::optional<Action> action;
//         if(game.currentPlayer == P1) {
//             Minimax search1(game, agent1, depth);
//             search1.run(mode);
//             action = search1.bestAction;
//         }
//         if(game.currentPlayer == P2) {
//             Minimax search2(game, agent2, depth);
//             search2.run(mode);
//             action = search2.bestAction;
//         }
//         if(action) {
//             std::cout << action.value().toString() << std::endl;
//             game.apply(action.value());
//         } else {
//             std::cout << "Player " << (int)game.currentPlayer << " did not find a suitable action" << std::endl;
//             break;
//         }
//     }
// }

// int main() {
//     // test1();
//     // test2();
//     // test3();
//     // test4();
//     // test5();
//     // test6();
//     // test7();
//     test8();
// }