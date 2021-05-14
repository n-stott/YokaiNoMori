#include <iostream>
#include "action.h"
#include "actionordering.h"
#include "enums.h"
#include <optional>
#include <string>
#include "gamestate.h"
#include "agent.h"
#include "minimax/minimax.h"
#include <iostream>
#include <cstring>

std::optional<ActionType> readActionType() {
    std::string command;
    try {
        std::cin >> command;
        if(command == "move") return std::make_optional(ActionType::Move);
        if(command == "drop") return std::make_optional(ActionType::Drop);
    } catch(...) {
        return std::nullopt;
    }
    return std::nullopt;
}

std::optional<PieceType> readPieceType() {
    std::string p;
    try {
        std::cin >> p;
        if(p == "k" || p == "K") return std::make_optional(PieceType::King);
        if(p == "r" || p == "R") return std::make_optional(PieceType::Rook);
        if(p == "t" || p == "T") return std::make_optional(PieceType::Tower);
        if(p == "p" || p == "P") return std::make_optional(PieceType::Pawn);
        if(p == "s" || p == "S") return std::make_optional(PieceType::SuperPawn);
    } catch(...) {
        return std::nullopt;
    }
    return std::nullopt;
}

std::optional<Pos> readBoardPosition() {
    std::string p;
    int x = -1;
    int y = -1;
    try {
        std::cin >> p;
        if(p.size() < 2) return std::nullopt;
        if(p[0] == 'A' || p[0] == 'a') y = 0;
        if(p[0] == 'B' || p[0] == 'b') y = 1;
        if(p[0] == 'C' || p[0] == 'c') y = 2;
        if(p[1] == '1') x = 0;
        if(p[1] == '2') x = 1;
        if(p[1] == '3') x = 2;
        if(p[1] == '4') x = 3;
        if(x != -1 && y != -1) {
            return std::make_optional(Pos(3*x+y));
        }
    } catch(...) {
        return std::nullopt;
    }
    return std::nullopt;
}

std::optional<uint8_t> readReservePosition() {
    std::string p;
    int x = -1;
    try {
        std::cin >> p;
        if(p.size() > 1) return std::nullopt;
        x = std::atoi(p.c_str());
        if(x != -1) {
            return std::make_optional(x);
        }
    } catch(...) {
        return std::nullopt;
    }
    return std::nullopt;
}

std::optional<Action> readAction(Color player) {
    std::optional<ActionType> command = readActionType();
    if(!command) return std::nullopt;

    if(command.value() == ActionType::Move) {
        std::optional<PieceType> piece = readPieceType();
        if(!piece) return std::nullopt;

        std::optional<Pos> src = readBoardPosition();
        if(!src) return std::nullopt;

        std::optional<Pos> dst = readBoardPosition();
        if(!dst) return std::nullopt;

        return std::make_optional(Action::move(Piece(piece.value(), player), src.value(), dst.value()));
    }

    if(command.value() == ActionType::Drop) {
        std::optional<PieceType> piece = readPieceType();
        if(!piece) return std::nullopt;

        std::optional<uint8_t> src = readReservePosition();
        if(!src) return std::nullopt;

        std::optional<Pos> dst = readBoardPosition();
        if(!dst) return std::nullopt;

        return std::make_optional(Action::drop(Piece(piece.value(), player), src.value(), dst.value()));
    }
    return std::nullopt;
}


void oneVsOne() {
    GameState state;
    while(!state.hasWinner()) {
        std::cout << state.niceToString() << std::endl;
        std::cout << "Turn of player : " << (state.currentPlayer == P1 ? "A" : "B") << std::endl;
        std::optional<Action> action = readAction(state.currentPlayer);
        if(action) {
            std::cout << action.value().toString() << std::endl;
            bool success = state.apply(action.value());
            std::cout << "move valid : " << success << std::endl;
        }
    }
}

template<Mode mode>
void oneVsAi() {
    int depth = 6;
    try {
        int newdepth = 5;
        std::cout << "Select difficulty (1--20) : ";
        std::cin >> newdepth;
        depth = std::max(0, std::min(20, newdepth));
    } catch(...) {
        depth = 6;
    }

    GameState state;
    Agent agent;

    using MyMinimax = Minimax<mode, Action, GameState, Agent, ActionOrdering>;

    while(!state.hasWinner()) {

        std::cout << state.niceToString() << std::endl;
        
        std::cout << "Turn of player : " << (state.currentPlayer == P1 ? "A" : "B") << std::endl;

        if(state.currentPlayer == P1) {
            std::optional<Action> action = readAction(state.currentPlayer);
            if(action) {
                std::cout << action.value().toString() << std::endl;
                bool success = state.apply(action.value());
                std::cout << "move valid : " << success << std::endl;
            }
        } else {
            std::optional<Action> action;
            MyMinimax search(state, agent, depth);
            search.run();
            action = search.bestAction;
            if(action) {
                state.apply(action.value());
            } else {
                std::cout << "Player " << (int)state.currentPlayer << " did not find a suitable action" << std::endl;
                break;
            }           
        }
    }
}


template<Mode mode>
void aivsAi() {
    GameState game;
    Agent agent1;
    Agent agent2;

    int depth = 6;
    try {
        int newdepth = 5;
        std::cout << "Select difficulty (1--20) : ";
        std::cin >> newdepth;
        depth = std::max(0, std::min(20, newdepth));
    } catch(...) {
        depth = 6;
    }

    using MyMinimax = Minimax<mode, Action, GameState, Agent, ActionOrdering>;

    while(!game.hasWinner()) {
        std::cout << game.niceToString() << std::endl;
        std::cout << "Turn of player : " << (game.currentPlayer == P1 ? "A" : "B") << std::endl;
        std::optional<Action> action;
        if(game.currentPlayer == P1) {
            MyMinimax search1(game, agent1, depth);
            search1.run();
            action = search1.bestAction;
        }
        if(game.currentPlayer == P2) {
            MyMinimax search2(game, agent2, depth);
            search2.run();
            action = search2.bestAction;
        }
        if(action) {
            std::cout << action.value().toString() << std::endl;
            game.apply(action.value());
        } else {
            std::cout << "Player " << (int)game.currentPlayer << " did not find a suitable action" << std::endl;
            break;
        }
    }
}

int main(int argc, char** argv) {
    std::cout << "Select game mode : 1v1, 1vAI or AIvAI" << std::endl;
    if(argc != 2) return 0;
    if(std::strcmp(argv[1], "1v1") == 0) {
        std::cout << "Starting 1v1 mode" << std::endl;
        oneVsOne();
    }
    if(std::strcmp(argv[1], "1vAI") == 0) {
        std::cout << "Starting 1vAI mode" << std::endl;
        oneVsAi<AlphaBeta>();
    }
    if(std::strcmp(argv[1], "AIvAI") == 0) {
        std::cout << "Starting AIvAI mode" << std::endl;
        aivsAi<PureMinimax>();
        // aivsAi<AlphaBeta>();
    }
    return 0;
}