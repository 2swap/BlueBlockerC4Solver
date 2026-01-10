#include "../solver/alphabeta.c"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/time.h>

// Get current time in milliseconds
long long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

// Time-limited alpha-beta search
// Returns the best result found within time_limit_ms milliseconds
typedef struct {
    C4Result result;
    int score;
    int depth_reached;
    long long nodes_searched;
    int pv_length;
    Move pv[100];
} SolverResult;

SolverResult solve_with_timeout(const C4Board* board, int time_limit_ms) {
    SolverResult result;
    result.result = RESULT_INCOMPLETE;
    result.score = 0;
    result.depth_reached = 0;
    result.nodes_searched = 0;
    result.pv_length = 0;

    long long start_time = get_time_ms();
    int max_depth = 50; // High limit, we'll stop when time runs out
    int alpha = -100000;
    int beta = 100000;

    // Iterative deepening: search depth 1, 2, 3, ... until time runs out
    for(int depth = 1; depth <= max_depth; depth++) {
        // Check if we have time left
        long long elapsed = get_time_ms() - start_time;
        if(elapsed >= time_limit_ms && depth > 1) {
            // Time's up, return best result found so far
            break;
        }

        node_count = 0;
        Move current_pv[100];
        int current_pv_length;

        // Search at current depth
        int score = C4Board_alpha_beta_internal(board, alpha, beta, 0, depth, current_pv, &current_pv_length);
        result.nodes_searched += node_count;

        // Update best result
        result.score = score;
        result.result = score_to_result(score);
        result.depth_reached = depth;
        result.pv_length = current_pv_length;
        for(int i = 0; i < current_pv_length; i++) {
            result.pv[i] = current_pv[i];
        }

        // If we found a forced mate, no need to search deeper
        if(score > 5000 || score < -5000) {
            break;
        }
    }

    return result;
}

void display_solver_result(const SolverResult* sr, const C4Board* board) {
    printf("\n--- Solver Analysis (depth %d, %lld nodes) ---\n",
           sr->depth_reached, sr->nodes_searched);

    if(sr->result == RESULT_RED) {
        printf("Position is WINNING for Red!\n");
    } else if(sr->result == RESULT_YELLOW) {
        printf("Position is WINNING for Yellow!\n");
    } else if(sr->result == RESULT_TIE) {
        printf("Position evaluates to a TIE.\n");
    } else {
        printf("Position is INCOMPLETE (not fully solved).\n");
    }

    // Show principal variation if we have one
    if(sr->pv_length > 0) {
        int mate_distance = (sr->pv_length + 1) / 2;
        if(sr->score > 5000) {
            printf("Forced mate in %d for Red:\n", mate_distance);
        } else if(sr->score < -5000) {
            printf("Forced mate in %d for Yellow:\n", mate_distance);
        } else {
            printf("Best line (%d moves):\n", sr->pv_length);
        }

        C4Board sim_board = *board;
        for(int i = 0; i < sr->pv_length && i < 10; i++) { // Show first 10 moves
            const char* player = C4Board_is_reds_turn(&sim_board) ? "Red" : "Yellow";
            if(sr->pv[i].blue_col == 0) {
                printf("  %d. %s: col %d\n", i+1, player, sr->pv[i].normal_col);
            } else {
                printf("  %d. %s: blue col %d, normal col %d\n",
                       i+1, player, sr->pv[i].blue_col, sr->pv[i].normal_col);
            }
            char illegal_reason[128];
            if(C4Board_is_legal(&sim_board, sr->pv[i].blue_col, sr->pv[i].normal_col, illegal_reason)) {
                C4Board_make_move(&sim_board, sr->pv[i].blue_col, sr->pv[i].normal_col);
            }
        }
        if(sr->pv_length > 10) {
            printf("  ... (%d more moves)\n", sr->pv_length - 10);
        }
    }
    printf("--------------------------------------\n");
}

void display_game_state(const C4Board* board) {
    printf("\n=== Current Game State ===\n");
    C4Board_print(board);
    printf("\nCurrent player: %s\n", C4Board_is_reds_turn(board) ? "Red" : "Yellow");
    printf("Red blue pieces remaining: %d\n", board->red_blue_remaining);
    printf("Yellow blue pieces remaining: %d\n", board->yellow_blue_remaining);

    C4Result winner = C4Board_who_won(board);
    if (winner == RESULT_RED) {
        printf("*** RED WINS! ***\n");
    } else if (winner == RESULT_YELLOW) {
        printf("*** YELLOW WINS! ***\n");
    } else if (winner == RESULT_TIE) {
        printf("*** GAME IS A TIE! ***\n");
    } else {
        printf("Game continues...\n");
    }
    printf("=========================\n");
}

void show_help() {
    printf("\nCommands:\n");
    printf("  1-7: Play a regular piece in column 1-7\n");
    printf("  1b 2: Play a blue piece in column 1, then a normal piece in column 2\n");
    printf("  help: Show this help message\n");
    printf("  quit: Exit the game\n");
    printf("  print: Show current board state\n");
    printf("\nNote: After each move, the solver will analyze the position.\n");
}

typedef enum {
    HUMAN_VS_HUMAN,
    HUMAN_VS_BOT
} GameMode;

void bot_make_move(C4Board* board, int time_limit_ms) {
    printf("\n=== Bot is thinking... ===\n");
    SolverResult sr = solve_with_timeout(board, time_limit_ms);
    display_solver_result(&sr, board);

    if(sr.pv_length > 0) {
        Move best_move = sr.pv[0];
        char illegal_reason[256];

        if(C4Board_is_legal(board, best_move.blue_col, best_move.normal_col, illegal_reason)) {
            if(best_move.blue_col == 0) {
                printf("\nBot plays: column %d\n", best_move.normal_col);
            } else {
                printf("\nBot plays: blue in column %d, normal in column %d\n",
                       best_move.blue_col, best_move.normal_col);
            }
            C4Board_make_move(board, best_move.blue_col, best_move.normal_col);
        } else {
            printf("ERROR: Bot tried illegal move! %s\n", illegal_reason);
            printf("Falling back to first legal move...\n");
            // Fallback: play first legal move
            for(int col = 1; col <= 7; col++) {
                if(C4Board_is_legal(board, 0, col, illegal_reason)) {
                    printf("Bot plays: column %d\n", col);
                    C4Board_make_move(board, 0, col);
                    break;
                }
            }
        }
    } else {
        printf("WARNING: Bot couldn't find any moves in PV!\n");
        printf("Playing first legal move...\n");
        // Fallback: play first legal move
        char illegal_reason[256];
        for(int col = 1; col <= 7; col++) {
            if(C4Board_is_legal(board, 0, col, illegal_reason)) {
                printf("Bot plays: column %d\n", col);
                C4Board_make_move(board, 0, col);
                break;
            }
        }
    }
}

int main() {
    C4Board board;
    C4Board_init(&board);
    char input[256];
    GameMode game_mode;
    int bot_player = 0; // 0 = no bot, 1 = bot is Red, 2 = bot is Yellow
    int time_limit_ms = 1000; // Default 1 second

    printf("Welcome to Interactive Connect 4 with Solver Analysis!\n");
    printf("\nGame Mode:\n");
    printf("  1. Human vs Human\n");
    printf("  2. Human vs Bot\n");
    printf("Select mode (1 or 2): ");

    if (!fgets(input, sizeof(input), stdin)) {
        return 1;
    }
    input[strcspn(input, "\r\n")] = 0;

    if(strcmp(input, "2") == 0) {
        game_mode = HUMAN_VS_BOT;
        printf("\nChoose your color:\n");
        printf("  1. Play as Red (you go first)\n");
        printf("  2. Play as Yellow (bot goes first)\n");
        printf("Select (1 or 2): ");

        if (!fgets(input, sizeof(input), stdin)) {
            return 1;
        }
        input[strcspn(input, "\r\n")] = 0;

        if(strcmp(input, "1") == 0) {
            bot_player = 2; // Bot is Yellow
            printf("\nYou are Red! You go first.\n");
        } else {
            bot_player = 1; // Bot is Red
            printf("\nYou are Yellow! Bot goes first.\n");
        }

        printf("\nBot thinking time (in milliseconds, default 1000): ");
        if (!fgets(input, sizeof(input), stdin)) {
            return 1;
        }
        input[strcspn(input, "\r\n")] = 0;
        if(strlen(input) > 0) {
            int parsed_time = atoi(input);
            if(parsed_time > 0) {
                time_limit_ms = parsed_time;
            }
        }
        printf("Bot will think for %d ms per move.\n", time_limit_ms);
    } else {
        game_mode = HUMAN_VS_HUMAN;
        printf("\nHuman vs Human mode selected.\n");
    }

    printf("\nType 'help' for commands.\n");
    display_game_state(&board);

    while (1) {
        C4Result winner = C4Board_who_won(&board);
        if (winner != RESULT_INCOMPLETE) {
            printf("\nGame Over! Type 'quit' to exit or any other key to start a new game.\n");
            if (!fgets(input, sizeof(input), stdin)) {
                break;
            }
            input[strcspn(input, "\r\n")] = 0; // trim newline
            if (strcmp(input, "quit") == 0) {
                break;
            } else {
                C4Board_init(&board); // Reset the board
                display_game_state(&board);
                continue;
            }
        }

        // Check if it's the bot's turn
        int is_bot_turn = (game_mode == HUMAN_VS_BOT && board.current_player == bot_player);

        if(is_bot_turn) {
            // Bot's turn
            bot_make_move(&board, time_limit_ms);
            display_game_state(&board);
            continue;
        }

        // Human's turn
        printf("\nEnter your move: ");
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        input[strcspn(input, "\r\n")] = 0; // trim newline

        // Convert to lowercase for easier parsing
        for (int i = 0; input[i]; i++) {
            input[i] = tolower((unsigned char)input[i]);
        }

        if (strcmp(input, "quit") == 0) {
            break;
        } else if (strcmp(input, "help") == 0) {
            show_help();
            continue;
        } else if (strcmp(input, "print") == 0) {
            display_game_state(&board);
            continue;
        }

        int one_index_blue_column = 0;
        int one_index_normal_column = 0;

        // Parse blue piece moves (e.g. "1b 2")
        if (strlen(input) == 4 &&
            input[1] == 'b' &&
            input[2] == ' ' &&
            input[0] >= '1' && input[0] <= '7' &&
            input[3] >= '1' && input[3] <= '7') {
            one_index_blue_column = input[0] - '0';
            one_index_normal_column = input[3] - '0';
        }
        // Parse regular moves (e.g. "1")
        else if (strlen(input) == 1 && input[0] >= '1' && input[0] <= '7') {
            one_index_normal_column = input[0] - '0';
        } else {
            printf("Invalid input. Type 'help' for valid commands.\n");
            continue;
        }

        printf("Processing move with blue column: %d and normal column: %d\n", one_index_blue_column, one_index_normal_column);
        char illegal_reason[256];
        if (!C4Board_is_legal(&board, one_index_blue_column, one_index_normal_column, illegal_reason)) {
            printf("Illegal move: %s\n", illegal_reason);
            continue;
        }

        C4Board_make_move(&board, one_index_blue_column, one_index_normal_column);

        display_game_state(&board);

        // Run solver analysis after human move (only in human vs human mode)
        if(game_mode == HUMAN_VS_HUMAN && C4Board_who_won(&board) == RESULT_INCOMPLETE) {
            printf("\nAnalyzing position...\n");
            SolverResult sr = solve_with_timeout(&board, time_limit_ms);
            display_solver_result(&sr, &board);
        }
    }

    printf("Thanks for playing!\n");
    return 0;
}
