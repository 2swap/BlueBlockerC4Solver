#include "../core/C4Board.c"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
}

int main() {
    C4Board board;
    C4Board_init(&board);
    char input[256];

    printf("Welcome to Interactive Connect 4!\n");
    printf("Type 'help' for commands.\n");

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
    }

    printf("Thanks for playing!\n");
    return 0;
}
