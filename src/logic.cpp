#include "gamestate.h"
#include "agent.h"
#include "actionordering.h"
#include "action.h"
#include "minimax/minimax.h"

// void test1() {
    // GameState s;
    // std::cout << s.niceToString() << '\n';
    // auto aa1 = s.allowedActions();
    // std::cout << aa1.toString() << '\n';
    // s.move(Pawn, P1, 7, 4);
    // std::cout << s.niceToString() << '\n';
    // auto aa2 = s.allowedActions();
    // std::cout << aa2.toString() << '\n';
    // s.move(King, P2, 1, 4);
    // std::cout << s.niceToString() << '\n';
    // auto aa3 = s.allowedActions();
    // std::cout << aa3.toString() << '\n';
// }

// void test5() {
//     GameState game("kK", "", "", P1);
//     std::cout << game.niceToString() << std::endl;

//     Agent agent1;
//     int depth = 1;
//     Minimax<PureMinimax, Action, GameState, Agent, ActionOrdering> search1(game, agent1, depth);
//     search1.run();
//     std::optional<Action> action = search1.bestAction;
//     if(action) {
//         std::cout << action.value().toString() << std::endl;
//         game.apply(action.value());
//     }

//     std::cout << game.niceToString() << std::endl;
// }

// void test6() {
//     GameState game("k", "K", "", P2);
//     std::cout << game.niceToString() << std::endl;
//     std::cout << game.hasWinner() << std::endl;
// }

// void test7() {
//     GameState game("TeeKReePkret", "", "P", P1);
//     std::cout << game.niceToString() << std::endl;
// }

// void test8() {
//     GameState game("eerKekeReTee", "stp", "", P2);
//     std::cout << game.niceToString() << std::endl;
//     Agent agent;
//     int depth = 6;
//     Minimax<PureMinimax, Action, GameState, Agent, ActionOrdering> search(game, agent, depth);
//     search.run();
// }

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