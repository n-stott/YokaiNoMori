#include "action.h"
#include "actionordering.h"
#include "enums.h"
#include <optional>
#include <string>
#include "gamestate.h"
#include "agent.h"
#include "minimax/minimax.h"
#include <cstring>

extern "C" {

    char board_buffer[256];
    char reserve0_buffer[256];
    char reserve1_buffer[256];

    void init() {
        std::fill(board_buffer, board_buffer+256, '\0');
        std::fill(reserve0_buffer, reserve0_buffer+256, '\0');
        std::fill(reserve1_buffer, reserve1_buffer+256, '\0');
    }

    int validAction(
        const char* board,
        const char* reserve1,
        const char* reserve2,
        int player,
        const char* actiontype,
        const char* piece,
        int start,
        int end
    ) {
        if(player < 0) player = 0;
        if(player > 1) player = 1;

        Piece p { piece[0] };
        ActionType type { strcmp(actiontype, "drop") == 0 ? ActionType::Drop : ActionType::Move };
        Pos src { static_cast<uint8_t>(start) };
        Pos dst { static_cast<uint8_t>(end) };

        GameHistory<Easy> history;
        GameState<Easy> state(&history, board, reserve1, reserve2, (Color)player);
        Action<Easy> action { p, type, src, dst };
        bool check = state.checkAction(action);

        return check;
    }

    int playAction(
        const char* board,
        const char* reserve1,
        const char* reserve2,
        int player,
        const char* actiontype,
        const char* piece,
        int start,
        int end
    ) {
        if(player < 0) player = 0;
        if(player > 1) player = 1;

        Piece p { piece[0] };
        ActionType type { strcmp(actiontype, "drop") == 0 ? ActionType::Drop : ActionType::Move };
        Pos src { static_cast<uint8_t>(start) };
        Pos dst { static_cast<uint8_t>(end) };

        GameHistory<Easy> history;
        GameState<Easy> state(&history, board, reserve1, reserve2, (Color)player);
        Action<Easy> action { p, type, src, dst };
        state.apply(action);

        init();

        std::strcpy(board_buffer, state.board.toString().c_str());
        std::strcpy(reserve0_buffer, state.reserve1.toString().c_str());
        std::strcpy(reserve1_buffer, state.reserve2.toString().c_str());

        return 1;
    }

    int searchBestMove(
        const char* board,
        const char* reserve1,
        const char* reserve2,
        int player,
        int depth
    ) {
        if(player < 0) player = 0;
        if(player > 1) player = 1;

        if(depth < 1) depth = 1;
        if(depth > 8) depth = 8;

        GameHistory<Easy> history;
        GameState<Easy> state(&history, board, reserve1, reserve2, (Color)player);

        Agent<Easy> agent;
        using MyMinimax = Minimax<Mode::IterativeDeepening, Action<Easy>, ActionSet<Easy>, GameState<Easy>, Agent<Easy>, ActionOrdering<Easy>>;

        std::optional<Action<Easy>> action;
        MyMinimax search(state, agent);
        search.run(depth);
        action = search.bestAction;
        if(action) {
            state.apply(action.value());
        } 

        init();
        
        std::strcpy(board_buffer, state.board.toString().c_str());
        std::strcpy(reserve0_buffer, state.reserve1.toString().c_str());
        std::strcpy(reserve1_buffer, state.reserve2.toString().c_str());

        return action.has_value();
    }

    const char* board() {
        return board_buffer;
    }

    const char* reserve0() {
        return reserve0_buffer;
    }

    const char* reserve1() {
        return reserve1_buffer;
    }


}