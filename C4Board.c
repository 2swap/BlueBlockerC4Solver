#pragma once

#include "Bitboard.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

const int BOARD_HEIGHT = 6;
const int BOARD_WIDTH = 7;
const Bitboard full_bitboard = 140185576636287ull;

// Note: these are ordered according to Red's heuristic value for alpha-beta pruning.
typedef enum {
    RESULT_YELLOW = 0,
    RESULT_TIE,
    RESULT_RED,
    RESULT_INCOMPLETE
} C4Result;

typedef enum {
    PIECETYPE_RED = 0,
    PIECETYPE_YELLOW = 1,
    PIECETYPE_BLUE = 2
} PieceType;

const char* result_to_string(C4Result r) {
    switch(r) {
        case RESULT_YELLOW: return "YELLOW";
        case RESULT_TIE: return "TIE";
        case RESULT_RED: return "RED";
        case RESULT_INCOMPLETE: return "INCOMPLETE";
    }
    return "UNKNOWN";
}

const char* disk_col(int i){
    // returns pointer to a string literal
    switch(i) {
        case 1: return "\033[31mx\033[0m";  // Red "x"
        case 2: return "\033[33mo\033[0m";  // Yellow "o"
        case 3: return "\033[34mB\033[0m";  // Blue "B"
        default: return ".";
    }
}

typedef struct {
    Bitboard red_bitboard, yellow_bitboard, blue_bitboard;
    int red_blue_remaining, yellow_blue_remaining;
    int current_player; // 1 for red, 2 for yellow
    int num_moves; // number of moves made so far
} C4Board;

void C4Board_init(C4Board* board) {
    board->red_bitboard = 0ULL;
    board->yellow_bitboard = 0ULL;
    board->blue_bitboard = 0ULL;
    board->red_blue_remaining = 2;
    board->yellow_blue_remaining = 2;
    board->current_player = 1;
    board->num_moves = 0;
}

int C4Board_piece_code_at(const C4Board* board, int x, int y) {
    return bitboard_at(board->red_bitboard, x, y) + 2*bitboard_at(board->yellow_bitboard, x, y) + 3*bitboard_at(board->blue_bitboard, x, y);
}

void C4Board_print(const C4Board* board) {
    for(int y = 0; y < BOARD_HEIGHT; y++) {
        for(int x = 0; x < BOARD_WIDTH; x++) {
            int code = C4Board_piece_code_at(board, x, y);
            printf("%s ", disk_col(code));
        }
        printf("\n");
    }
}

Bitboard C4Board_legal_moves(const C4Board* board) {
    return downset(board->red_bitboard | board->yellow_bitboard | board->blue_bitboard);
}

// Check if a column is full
int C4Board_column_is_full(const C4Board* board, int one_index_column) {
    return (((board->red_bitboard|board->yellow_bitboard|board->blue_bitboard) >> (one_index_column-1)) & 1ul) != 0ul;
}

C4Result C4Board_who_won(const C4Board* board) {
    const int v = BOARD_WIDTH;
    const int w = BOARD_WIDTH + 1;
    const int x = BOARD_WIDTH + 2;
    if((board->red_bitboard | board->yellow_bitboard | board->blue_bitboard) == full_bitboard)
        return RESULT_TIE;

    // Check for red and yellow wins
    for(int i=0; i < 2; i++) {
        Bitboard b = (i == 0) ? board->red_bitboard : board->yellow_bitboard;
        // horizontal, vertical and diagonal checks
        if( (b & (b >> 1) & (b >> (2*1)) & (b >> (3*1))) ||
            (b & (b >> w) & (b >> (2*w)) & (b >> (3*w))) ||
            (b & (b >> v) & (b >> (2*v)) & (b >> (3*v))) ||
            (b & (b >> x) & (b >> (2*x)) & (b >> (3*x))) ) {
            return (i == 0) ? RESULT_RED : RESULT_YELLOW;
        }
    }
    return RESULT_INCOMPLETE;
}

int C4Board_is_solution(const C4Board* board) {
    C4Result winner = C4Board_who_won(board);
    return winner == RESULT_RED || winner == RESULT_YELLOW;
}

int C4Board_can_play_blue(const C4Board* board) {
    if(board->current_player == 1) return board->red_blue_remaining > 0;
    else return board->yellow_blue_remaining > 0;
}

int C4Board_get_blue_remaining(const C4Board* board) {
    if(board->current_player == 1) return board->red_blue_remaining;
    else return board->yellow_blue_remaining;
}

int C4Board_is_reds_turn(const C4Board* board) {
    return board->current_player == 1;
}

void C4Board_play_piece(C4Board* board, PieceType pt, int one_index_column) {
    Bitboard legal = C4Board_legal_moves(board);
    Bitboard column_mask = make_column(one_index_column-1);
    Bitboard p = legal & column_mask;

    if(pt != PIECETYPE_BLUE)
        board->current_player = (board->current_player == 1) ? 2 : 1;

    switch(pt) {
        case PIECETYPE_RED:
            board->red_bitboard |= p;
            break;
        case PIECETYPE_YELLOW:
            board->yellow_bitboard |= p;
            break;
        case PIECETYPE_BLUE:
            board->blue_bitboard |= p;
            if(board->current_player == 2) {
                board->red_blue_remaining -= 1;
            } else {
                board->yellow_blue_remaining -= 1;
            }
            break;
    }
}

int C4Board_is_legal(const C4Board* board, int one_index_blue_piece, int one_index_normal_piece) {
    // Validate blue piece move
    if(one_index_blue_piece < 0 || one_index_blue_piece > BOARD_WIDTH) {
        //sprintf(illegal_reason, "Blue piece in illegal column (%d)", one_index_blue_piece);
        return 0;
    }
    if(one_index_blue_piece != 0) {
        if(C4Board_column_is_full(board, one_index_blue_piece)) {
            //sprintf(illegal_reason, "Tried playing blue piece in full column");
            return 0;
        }
        if(!C4Board_can_play_blue(board)) {
            //sprintf(illegal_reason, "Attempted to play blue piece when none remaining for current player");
            return 0;
        }
        C4Board simulated_board = *board;
        C4Board_play_piece(&simulated_board, PIECETYPE_BLUE, one_index_blue_piece);
    }

    // Validate normal piece move
    if(one_index_normal_piece < 1 || one_index_normal_piece > BOARD_WIDTH) {
        //strcpy(illegal_reason, "Normal piece in illegal column");
        return 0;
    }
    if(C4Board_column_is_full(board, one_index_normal_piece)) {
        //strcpy(illegal_reason, "Tried playing illegal normal piece in full column");
        return 0;
    }

    return 1;
}

void C4Board_make_move(C4Board* board, int one_index_blue_piece, int one_index_normal_piece) {
    // A move consists of an optional blue piece and a required normal piece.
    // A blue piece value of 0 means no blue piece is played.
    //char illegal_reason[128];
    if(!C4Board_is_legal(board, one_index_blue_piece, one_index_normal_piece)) {
        C4Board_print(board);
        fprintf(stderr, "Attempted to play illegal move: %s, blue piece: %d, normal piece: %d\n", /*illegal_reason, */one_index_blue_piece, one_index_normal_piece);
        exit(EXIT_FAILURE);
    }

    if(one_index_blue_piece != 0) {
        C4Board_play_piece(board, PIECETYPE_BLUE, one_index_blue_piece);
    }
    C4Board_play_piece(board, board->current_player == 1 ? PIECETYPE_RED : PIECETYPE_YELLOW, one_index_normal_piece);
    board->num_moves++;
}

C4Board C4Board_child(const C4Board* board, int one_index_blue_piece, int one_index_normal_piece) {
    C4Board new_board = *board;
    C4Board_make_move(&new_board, one_index_blue_piece, one_index_normal_piece);
    return new_board;
}

typedef struct {
    int size;
    int data[8];
} IntArray;

IntArray naive_move_ordering_blue = {8, {0,4,5,3,2,6,1,7}};
IntArray naive_move_ordering_normal = {7, {4,5,3,2,6,1,7}};

int C4Board_alpha_beta(const C4Board* board, int alpha, int beta) {
    C4Result result = C4Board_who_won(board);
         if(result == RESULT_TIE   ) return 0;
    else if(result == RESULT_RED   ) return  43-board->num_moves;
    else if(result == RESULT_YELLOW) return -43+board->num_moves;

    if(board->current_player == 1) {
        int best_result = -100;
        for(int i = 0; i < naive_move_ordering_blue.size; i++) {
            int one_index_blue_piece = naive_move_ordering_blue.data[i];
            for(int j = 0; j < naive_move_ordering_normal.size; j++) {
                int one_index_normal_piece = naive_move_ordering_normal.data[j];
                if(C4Board_is_legal(board, one_index_blue_piece, one_index_normal_piece)) {
                    C4Board child_board = C4Board_child(board, one_index_blue_piece, one_index_normal_piece);
                    int child_result = C4Board_alpha_beta(&child_board, alpha, beta);
                    if(child_result > best_result) best_result = child_result;
                    if(best_result >= beta) break; // Beta cut-off
                    if(alpha < (int)best_result) alpha = (int)best_result;
                }
            }
        }
        return best_result;
    } else {
        int best_result = 100;
        for(int i = 0; i < naive_move_ordering_blue.size; i++) {
            int one_index_blue_piece = naive_move_ordering_blue.data[i];
            for(int j = 0; j < naive_move_ordering_normal.size; j++) {
                int one_index_normal_piece = naive_move_ordering_normal.data[j];
                if(C4Board_is_legal(board, one_index_blue_piece, one_index_normal_piece)) {
                    C4Board child_board = C4Board_child(board, one_index_blue_piece, one_index_normal_piece);
                    int child_result = C4Board_alpha_beta(&child_board, alpha, beta);
                    if(child_result < best_result) best_result = child_result;
                    if(best_result <= alpha) break; // Alpha cut-off
                    if(beta > (int)best_result) beta = (int)best_result;
                }
            }
        }
        return best_result;
    }
}

C4Result C4Board_alpha_beta_simple(const C4Board* board) {
    return C4Board_alpha_beta(board, -100, 100);
}

