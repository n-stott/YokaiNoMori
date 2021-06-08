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

#define ENABLE_HUMAN_PLAYER 1

#if ENABLE_HUMAN_PLAYER
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
        if(p == "b" || p == "B") return std::make_optional(PieceType::Bishop);
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

template<BoardConfig config>
std::optional<Action<config>> readAction(Color player) {
    std::optional<ActionType> command = readActionType();
    if(!command) return std::nullopt;

    if(command.value() == ActionType::Move) {
        std::optional<PieceType> piece = readPieceType();
        if(!piece) return std::nullopt;

        std::optional<Pos> src = readBoardPosition();
        if(!src) return std::nullopt;

        std::optional<Pos> dst = readBoardPosition();
        if(!dst) return std::nullopt;

        return std::make_optional(Action<config>::move(Piece(piece.value(), player), src.value(), dst.value()));
    }

    if(command.value() == ActionType::Drop) {
        std::optional<PieceType> piece = readPieceType();
        if(!piece) return std::nullopt;

        std::optional<uint8_t> src = readReservePosition();
        if(!src) return std::nullopt;

        std::optional<Pos> dst = readBoardPosition();
        if(!dst) return std::nullopt;

        return std::make_optional(Action<config>::drop(Piece(piece.value(), player), src.value(), dst.value()));
    }
    return std::nullopt;
}


void oneVsOne() {
    GameHistory history;
    GameState<Easy> state(&history);
    while(!state.hasWinner()) {
        Logger::log(Verb::Std, [&]() {
            std::string s;
            s += state.niceToString() + '\n';
            s += "Turn of player : ";
            s += (state.currentPlayer == P1 ? "A" : "B");
            s += '\n';
            return s;
        });
        std::optional<Action<Easy>> action = readAction<Easy>(state.currentPlayer);
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

    GameHistory history;
    GameState<Easy> state(&history);
    Agent agent;

    using MyMinimax = Minimax<mode, Action<Easy>, ActionSet<Easy>, GameState<Easy>, Agent, ActionOrdering<Easy>>;

    while(!state.gameOver()) {
        Logger::log(Verb::Std, [&]() { return state.niceToString(); });
        Logger::log(Verb::Std, [&]() { return std::string{"Turn of player : "} + (state.currentPlayer == P1 ? "A" : "B"); });

        if(state.currentPlayer == P1) {
            std::optional<Action<Easy>> action = readAction<Easy>(state.currentPlayer);
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
            std::optional<Action<Easy>> action;
            MyMinimax search(state, agent);
            search.run(depth);
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
#endif


template<Mode mode, BoardConfig config>
void aivsAi(int depth1, int depth2) {

    Logger::log(Verb::Std, [&](){
        std::stringstream ss;
        ss << "Starting AIvAI mode with :\n";
        ss << " depths : " + std::to_string(depth1) + " vs " + std::to_string(depth2) << "\n";
        ss << " mode   : " << (mode == Mode::PureMinimax ? "pure" : (mode == AlphaBeta ? "Minimax" : "Iterative deepening")) << "\n";
        ss << " config : " << (config == Easy ? "easy" : "medium") << "\n";
        return ss.str();
    });

    GameHistory history;
    GameState<config> game(&history);
    Agent agent1;
    Agent agent2;

    depth1 = std::max(0, std::min(20, depth1));
    depth2 = std::max(0, std::min(20, depth2));

    using MyMinimax = Minimax<mode, Action<config>, ActionSet<config>, GameState<config>, Agent, ActionOrdering<config>>;

    while(!game.gameOver()) {
        Logger::log(Verb::Std,
            [&]() {
                std::string s;
                s += game.niceToString() + '\n';
                s += "Turn of player : ";
                s += (game.currentPlayer == P1 ? "A" : "B");
                s += '\n';
                return s;
            });
        std::optional<Action<config>> action;
        if(game.currentPlayer == P1) {
            MyMinimax search1(game, agent1);
            search1.run(depth1);
            action = search1.bestAction;
        }
        if(game.currentPlayer == P2) {
            MyMinimax search2(game, agent2);
            search2.run(depth2);
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
    if(argc <= 1) {
        Logger::log(Verb::Std, []() {
            return "Available game modes : --1v1, --1vAI, --AIvAI";
        });
        return 0;
    }
#if ENABLE_HUMAN_PLAYER
    if(std::strcmp(argv[1], "--1v1") == 0) {
        Logger::log(Verb::Std, [](){
            return "Starting 1v1 mode";
        });
        oneVsOne();
    }
    if(std::strcmp(argv[1], "--1vAI") == 0) {
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
#endif
    if(std::strcmp(argv[1], "--AIvAI") == 0) {
        if(argc <= 3) {
            Logger::log(Verb::Std, [](){
                return "Usage : exe --AIvAI depth1 depth2 [mode] [board]";
            });
            return 0;
        }
        int d1 = std::atoi(argv[2]);
        int d2 = std::atoi(argv[3]);
        Logger::log(Verb::Std, [&](){
            std::stringstream ss;
            ss << "Starting AIvAI mode with :\n";
            ss << " depths : " + std::to_string(d1) + " vs " + std::to_string(d2) << "\n";
            ss << " mode   : " << (argc <= 4 ? "pure" : argv[4]) << "\n";
            ss << " config : " << (argc <= 5 ? "easy" : argv[5]) << "\n";
            return ss.str();
        });
        if(argc >= 4 || (argc == 5 && std::strcmp(argv[4], "pure") == 0)) {
            if(argc < 6 || (argc == 6 && std::strcmp(argv[5], "easy") == 0)) {
                aivsAi<PureMinimax, BoardConfig::Easy>(d1, d2);
            }
            if((argc == 6 && std::strcmp(argv[5], "medium") == 0)) {
                aivsAi<PureMinimax, BoardConfig::Medium>(d1, d2);
            }
        }
        if(argc >= 5 && std::strcmp(argv[4], "alphabeta") == 0) {
            if(argc < 6 || (argc == 6 && std::strcmp(argv[5], "easy") == 0)) {
                aivsAi<AlphaBeta, BoardConfig::Easy>(d1, d2);
            }
            if((argc == 6 && std::strcmp(argv[5], "medium") == 0)) {
                aivsAi<AlphaBeta, BoardConfig::Medium>(d1, d2);
            }
        }
        if(argc >= 5 && std::strcmp(argv[4], "iterdeepen") == 0) {
            if(argc < 6 || (argc == 6 && std::strcmp(argv[5], "easy") == 0)) {
                aivsAi<IterativeDeepening, BoardConfig::Easy>(d1, d2);
            }
            if((argc == 6 && std::strcmp(argv[5], "medium") == 0)) {
                aivsAi<IterativeDeepening, BoardConfig::Medium>(d1, d2);
            }
        }
    }
    if(argc >= 1 && std::strcmp(argv[1], "--interactive") == 0) {
        if(argc <= 6) {
            Logger::log(Verb::Std, [](){
                return "Usage : exe --interactive boardstate reserve1 reserve2 currentplayer depth\nempty reserve = '.'";
            });
            return 0;
        }

        if(std::strlen(argv[2]) != 12) {
            Logger::log(Verb::Std, []() { return "invalid board : must have 12 characters"; });
        }

        if(std::strlen(argv[3]) > 7) {
            Logger::log(Verb::Std, []() { return "invalid reserve 1 : cannot have more than 7 pieces"; });
        }

        if(std::strlen(argv[4]) > 7) {
            Logger::log(Verb::Std, []() { return "invalid reserve 2 : cannot have more than 7 pieces"; });
        }

        if (std::strlen(argv[5]) > 1) {
            Logger::log(Verb::Std, []() { return "invalid player"; });
            return 0;
        }
        char p = argv[5][0];
        if (p != 'A' && 
            p != 'a' && 
            p != 'B' && 
            p != 'b') {
            Logger::log(Verb::Std, []() { return "invalid player"; });
            return 0;
        }
        Color player = (p == 'A' || p == 'a') ? Color::P1 : Color::P2;

        int depth = std::atoi(argv[6]);

        GameHistory history;

        GameState<Easy> state(&history, argv[2], argv[3], argv[4], player);


        Logger::log(Verb::Std, [&]() {
            return state.niceToString();
        });

        Agent agent;
        using MyMinimax = Minimax<Mode::AlphaBeta, Action<Easy>, ActionSet<Easy>, GameState<Easy>, Agent, ActionOrdering<Easy>>;

        std::optional<Action<Easy>> action;
        MyMinimax search(state, agent);
        search.run(depth);
        action = search.bestAction;
        if(action) {
            Logger::log(Verb::Std, [&](){ return action->toString(); });
        } else {
            Logger::log(Verb::Std, [&]() {
                std::string s;
                s += "Player ";
                s += p;
                s += " did not find a suitable action";
                return s;
            });
        }

        Logger::log(Verb::Dev, [&]() {
            return state.niceToString();
        });
    }
    return 0;
}
