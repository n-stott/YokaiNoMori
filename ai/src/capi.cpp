#include "action.h"
#include "actionordering.h"
#include "enums.h"
#include <optional>
#include <string>
#include "gamestate.h"
#include "gamehistory.h"
#include "agent.h"
#include "minimax/minimax.h"
#include <cstring>
#include <memory>

extern "C" {

    struct Buffers {
        char board_buffer[256];
        char reserve0_buffer[256];
        char reserve1_buffer[256];

        Buffers() {
            std::fill(board_buffer, board_buffer+256, '\0');
            std::fill(reserve0_buffer, reserve0_buffer+256, '\0');
            std::fill(reserve1_buffer, reserve1_buffer+256, '\0');
        }

        void read(const GameState* state) {
            std::strcpy(board_buffer, state->board.toString().c_str());
            std::strcpy(reserve0_buffer, state->reserve0.toString().c_str());
            std::strcpy(reserve1_buffer, state->reserve1.toString().c_str());
        }
    };

    std::unique_ptr<GameHistory> history_;
    std::unique_ptr<GameState> state_;
    Buffers buffers_;

    void init() {
        history_ = std::make_unique<GameHistory>();
        state_ = std::make_unique<GameState>(history_.get());
        buffers_.read(state_.get());
    }

    void set_state(const char* board, const char* reserve0, const char* reserve1, int player) {
        if(player < 0) player = 0;
        if(player > 1) player = 1;

        history_ = std::make_unique<GameHistory>();
        state_ = std::make_unique<GameState>(history_.get(), board, reserve0, reserve1, (Color)player);
        buffers_.read(state_.get());
    }

    int validAction(const char* actiontype, const char* piece, int start, int end) {
        Piece p { piece[0] };
        ActionType type { strcmp(actiontype, "drop") == 0 ? ActionType::Drop : ActionType::Move };
        Pos src { static_cast<uint8_t>(start) };
        Pos dst { static_cast<uint8_t>(end) };

        Action action { p, type, src, dst };
        bool check = state_->checkAction(action);
        return check;
    }

    int playAction(const char* actiontype, const char* piece, int start, int end) {
        Piece p { piece[0] };
        ActionType type { strcmp(actiontype, "drop") == 0 ? ActionType::Drop : ActionType::Move };
        Pos src { static_cast<uint8_t>(start) };
        Pos dst { static_cast<uint8_t>(end) };

        Action action { p, type, src, dst };
        state_->apply(action);
        buffers_.read(state_.get());
        return 1;
    }

    int searchBestMove(int depth) {
        if(depth < 1) depth = 1;
        if(depth > 8) depth = 8;

        Agent agent;
        using MyMinimax = Minimax<Mode::IterativeDeepening, Action, ActionSet, GameState, Agent, ActionOrdering>;

        std::optional<Action> action;
        MyMinimax search(*state_, agent);
        search.run(depth);
        action = search.bestAction;
        if(action) {
            state_->apply(action.value());
        } 

        buffers_.read(state_.get());

        return action.has_value();
    }

    const char* board() {
        return buffers_.board_buffer;
    }

    const char* reserve0() {
        return buffers_.reserve0_buffer;
    }

    const char* reserve1() {
        return buffers_.reserve1_buffer;
    }


}