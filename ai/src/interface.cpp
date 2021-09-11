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
#include <ostream>
#include <fstream>

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
        if(p.size() < 1) return std::nullopt;
        char pc = p[0];
        if(pc == 'k' || pc == 'K') return std::make_optional(PieceType::King);
        if(pc == 'b' || pc == 'B') return std::make_optional(PieceType::Bishop);
        if(pc == 'r' || pc == 'R') return std::make_optional(PieceType::Rook);
        if(pc == 'p' || pc == 'P') return std::make_optional(PieceType::Pawn);
        if(pc == 'q' || pc == 'Q') return std::make_optional(PieceType::Queen);
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
        const char pc0 = p[0];
        const char pc1 = p[1];
        if(pc0 == 'A' || pc0 == 'a') y = 0;
        if(pc0 == 'B' || pc0 == 'b') y = 1;
        if(pc0 == 'C' || pc0 == 'c') y = 2;
        if(pc1 == '1') x = 0;
        if(pc1 == '2') x = 1;
        if(pc1 == '3') x = 2;
        if(pc1 == '4') x = 3;
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
    GameHistory history;
    GameState state(&history);
    while(!state.hasWinner()) {
        Logger::log(Verb::Std, [&]() {
            std::string s;
            s += state.niceToString() + '\n';
            s += "Turn of player : ";
            s += (state.currentPlayer == P1 ? 'A' : 'B');
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

    GameHistory history;
    GameState state(&history);
    Agent agent;

    using MyMinimax = Minimax<mode, Action, ActionSet, GameState, Agent, ActionOrdering>;

    while(!state.gameOver()) {
        Logger::log(Verb::Std, [&]() { return state.niceToString(); });
        Logger::log(Verb::Std, [&]() { return std::string{"Turn of player : "} + (state.currentPlayer == P1 ? 'A' : 'B'); });

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



template<Mode mode>
Color aivsAiFrom(
    Board b,
    Reserve<P0, GameConfig::ressize> r0,
    Reserve<P1, GameConfig::ressize> r1,
    Color player,
    int depth0, 
    int depth1) {

    Logger::log(Verb::Dev, [&](){
        std::stringstream ss;
        ss << "Starting AIvAI mode with :\n";
        ss << " depths : " + std::to_string(depth0) + " vs " + std::to_string(depth1) << '\n';
        ss << " mode   : " << (mode == Mode::PureMinimax ? "pure" : (mode == AlphaBeta ? "AlphaBeta" : "Iterative deepening")) << '\n';
        return ss.str();
    });

    GameHistory history;
    GameState game(&history, b, r0, r1, player);
    Agent agent0;
    Agent agent1;

    depth0 = std::max(0, std::min(20, depth0));
    depth1 = std::max(0, std::min(20, depth1));

    using MyMinimax = Minimax<mode, Action, ActionSet, GameState, Agent, ActionOrdering>;

    while(!game.gameOver()) {
        Logger::log(Verb::Std,
            [&]() {
                std::string s;
                s += game.niceToString() + '\n';
                s += "Turn of player : ";
                s += (game.currentPlayer == P0 ? 'A' : 'B');
                s += '\n';
                return s;
            });
        std::optional<Action> action;
        if(game.currentPlayer == P0) {
            MyMinimax search0(game, agent0);
            search0.run(depth0);
            action = search0.bestAction;
        }
        if(game.currentPlayer == P1) {
            MyMinimax search1(game, agent1);
            search1.run(depth1);
            action = search1.bestAction;
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
    
    Color winner = Color::None;
    if(game.hasWon(Color::P0)) {
        winner = Color::P0;
    } else if(game.hasWon(Color::P1)) {
        winner = Color::P1;
    }

    Logger::log(Verb::Std, [&]() {
        if(winner == Color::P0) {
            return "Player A has won";
        } else if(winner == Color::P1) {
            return "Player B has won";
        } else {
            return "Draw";
        }
    });

    return winner;

}


template<Mode mode>
Color aivsAi(int depth0, int depth1) {
    Board b;
    Reserve<P0, GameConfig::ressize> r0;
    Reserve<P1, GameConfig::ressize> r1;
    Color player = Color::P0;
    return aivsAiFrom<mode>(b, r0, r1, player, depth0, depth1);
}

template<typename Func>
void enumeratePositionsHelper(
                            unsigned int maxdepth,
                            GameState root,
                            std::ostream& ostream,
                            Func runner) {
    if(maxdepth == 0) return;
    ActionSet actions;
    root.fillAllowedActions(&actions);
    for(Action a : actions) {
        GameState node = root;
        node.apply(a);
        if(node.hasWinner()) continue;

        runner(node, ostream);

        

        enumeratePositionsHelper(maxdepth-1, node, ostream, runner);
    }
}

void enumeratePositions(unsigned int maxdepth, std::string filename) {

    GameHistory history;
    GameState game(&history);

    std::ofstream ofile;
    ofile.open(filename);

    auto runner = [](GameState node, std::ostream& ostream) {
        for(int i = 0; i < 2; ++i) {
            ostream << node.toString2() << " " << i;
            // Color winner = aivsAiFrom<AlphaBeta, Easy>(
            //     node.board,
            //     node.reserve1,
            //     node.reserve2,
            //     (Color)i,
            //     4,
            //     4
            // );
            // ostream << " " << winner;
            ostream << '\n';
            ostream << std::flush;
        }
    };

    enumeratePositionsHelper(maxdepth, game, ofile, runner);

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
    if(std::strcmp(argv[1], "--enumerate") == 0) {
        if(argc <= 2) {
            Logger::log(Verb::Std, [](){
                return "Usage : exe --enumerate [depth = 3] [filename = positions.txt]";
            });
        }
        unsigned int maxdepth = 3;
        if(argc >= 3) {
            maxdepth = std::atoi(argv[2]);
        }
        std::string filename = "positions.txt";
        if(argc >= 4) {
            filename = argv[3];
        }
        
        enumeratePositions(maxdepth, filename);
    }


    if(std::strcmp(argv[1], "--AIvAI") == 0) {
        if(argc <= 3) {
            Logger::log(Verb::Std, [](){
                return "Usage : exe --AIvAI depth0 depth1 [mode = pure|alphabeta|iterdeepen]";
            });
            return 0;
        }
        int d0 = std::atoi(argv[2]);
        int d1 = std::atoi(argv[3]);
        Logger::log(Verb::Std, [&](){
            std::stringstream ss;
            ss << "Starting AIvAI mode with :\n";
            ss << " depths : " + std::to_string(d0) + " vs " + std::to_string(d1) << '\n';
            ss << " mode   : " << (argc <= 4 ? "pure" : argv[4]) << '\n';
            ss << " config : " << (argc <= 5 ? "easy" : argv[5]) << '\n';
            return ss.str();
        });
        if(argc >= 5 && std::strcmp(argv[4], "alphabeta") == 0) {
            if(argc < 6 || (argc == 6 && std::strcmp(argv[5], "easy") == 0)) {
                aivsAi<AlphaBeta>(d0, d1);
                return 0;
            }
        }
        if(argc >= 5 && std::strcmp(argv[4], "iterdeepen") == 0) {
            if(argc < 6 || (argc == 6 && std::strcmp(argv[5], "easy") == 0)) {
                aivsAi<IterativeDeepening>(d0, d1);
                return 0;
            }
        }
        if(argc >= 4 || (argc == 5 && std::strcmp(argv[4], "pure") == 0)) {
            if(argc < 6 || (argc == 6 && std::strcmp(argv[5], "easy") == 0)) {
                aivsAi<PureMinimax>(d0, d1);
                return 0;
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
        Color player = (p == 'A' || p == 'a') ? Color::P0 : Color::P1;

        int depth = std::atoi(argv[6]);

        GameHistory history;

        GameState state(&history, argv[2], argv[3], argv[4], player);


        Logger::log(Verb::Std, [&]() {
            return state.niceToString();
        });

        Agent agent;
        using MyMinimax = Minimax<Mode::AlphaBeta, Action, ActionSet, GameState, Agent, ActionOrdering>;

        std::optional<Action> action;
        MyMinimax search(state, agent);
        search.run(depth);
        action = search.bestAction;
        if(action) {
            Logger::log(Verb::Std, [&](){ return action->toString(); });
            state.apply(action.value());
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
