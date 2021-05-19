#include "action.h"
#include "actionordering.h"
#include "enums.h"
#include <optional>
#include <string>
#include "gamestate.h"
#include "agent.h"
#include "minimax/minimax.h"
#include "minimax/logger.h"
#include <cstring>
#include <iostream>

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
        Logger::log(Verb::Std, [&]() {
            std::string s;
            s += state.niceToString() + '\n';
            s += "Turn of player : ";
            s += (state.currentPlayer == P1 ? "A" : "B");
            s += '\n';
            return s;
        });
        std::optional<Action> action = readAction(state.currentPlayer);
        if(action) {
            Logger::log(Verb::Dev, [&]() { return action.value().toString(); });
            bool ok = state.checkAction(action.value());
            if(!ok) {
                Logger::log(Verb::Std, [&](){ return "invalid move"; });
                continue;
            }
            bool success = state.apply(action.value());
            Logger::log(Verb::Dev, [&](){ return "move success : " + std::to_string(success); });
        }
    }
}

template<Mode mode>
void oneVsAi(int depth) {
    depth = std::max(0, std::min(20, depth));

    GameState state;
    Agent agent;

    using MyMinimax = Minimax<mode, Action, GameState, Agent, ActionOrdering>;

    while(!state.hasWinner()) {
        Logger::log(Verb::Std, [&]() { return state.niceToString(); });
        Logger::log(Verb::Std, [&]() { return std::string{"Turn of player : "} + (state.currentPlayer == P1 ? "A" : "B"); });

        if(state.currentPlayer == P1) {
            std::optional<Action> action = readAction(state.currentPlayer);
            if(action) {
                Logger::log(Verb::Dev, [&]() { return action.value().toString(); });
                bool ok = state.checkAction(action.value());
                if(!ok) {
                    Logger::log(Verb::Std, [&](){ return "invalid move"; });
                    continue;
                }
                bool success = state.apply(action.value());
                Logger::log(Verb::Dev, [&](){ return "move success : " + std::to_string(success); });
            }
        } else {
            std::optional<Action> action;
            MyMinimax search(state, agent, depth);
            search.run();
            action = search.bestAction;
            if(action) {
                state.apply(action.value());
            } else {
            Logger::log(Verb::Std, [&]() {
                std::string s;
                s += "Player ";
                s += (char)('A'+(int)state.currentPlayer);
                s += " did not find a suitable action";
                return s;
            });
                break;
            }           
        }
    }
}


template<Mode mode>
void aivsAi(int depth1, int depth2) {
    GameState game;
    Agent agent1;
    Agent agent2;

    depth1 = std::max(0, std::min(20, depth1));
    depth2 = std::max(0, std::min(20, depth2));

    using MyMinimax = Minimax<mode, Action, GameState, Agent, ActionOrdering>;

    while(!game.hasWinner()) {
        Logger::log(Verb::Std,
            [&]() {
                std::string s;
                s += game.niceToString() + '\n';
                s += "Turn of player : ";
                s += (game.currentPlayer == P1 ? "A" : "B");
                s += '\n';
                return s;
            });
        std::optional<Action> action;
        if(game.currentPlayer == P1) {
            MyMinimax search1(game, agent1, depth1);
            search1.run();
            action = search1.bestAction;
        }
        if(game.currentPlayer == P2) {
            MyMinimax search2(game, agent2, depth2);
            search2.run();
            action = search2.bestAction;
        }
        if(action) {
            Logger::log(Verb::Std, [&]() { return action.value().toString(); });
            game.apply(action.value());
        } else {
            Logger::log(Verb::Std, [&]() {
                std::string s;
                s += "Player ";
                s += (char)('A'+(int)game.currentPlayer);
                s += " did not find a suitable action";
                return s;
            });
            break;
        }
    }

    Logger::log(Verb::Std, [&]() {
        return game.niceToString();
    });
    
    Logger::log(Verb::Std, [&]() {
        if(game.hasWon(Color::P1)) {
            return "Player A has won";
        } else if(game.hasWon(Color::P2)) {
            return "Player B has won";
        } else {
            return "Draw";
        }
    });

}

int main(int argc, char** argv) {
    if(argc <= 1) return 0;
    if(std::strcmp(argv[1], "1v1") == 0) {
        Logger::log(Verb::Std, [](){
            return "Starting 1v1 mode";
        });
        oneVsOne();
    }
    if(std::strcmp(argv[1], "1vAI") == 0) {
        if(argc <= 2) {
            Logger::log(Verb::Std, [](){
                return "Usage : exe 1vAI depth";
            });
            return 0;
        }
        int depth = std::atoi(argv[2]);
        Logger::log(Verb::Std, [&](){
            return "Starting 1vAI mode vs depth : " + std::to_string(depth);
        });

        // oneVsAi<PureMinimax>(depth);
        oneVsAi<AlphaBeta>(depth);
    }
    if(std::strcmp(argv[1], "AIvAI") == 0) {
        if(argc <= 3) {
            Logger::log(Verb::Std, [](){
                return "Usage : exe AIvAI depth1 depth2";
            });
            return 0;
        }
        int d1 = std::atoi(argv[2]);
        int d2 = std::atoi(argv[3]);
        Logger::log(Verb::Std, [&](){
            return "Starting AIvAI mode with depths : " + std::to_string(d1) + " vs " + std::to_string(d2);
        });
        // aivsAi<PureMinimax>(d1, d2);
        aivsAi<AlphaBeta>(d1, d2);
    }
    return 0;
}