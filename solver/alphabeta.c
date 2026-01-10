#pragma once

#include "../core/C4Board.c"

// Move ordering strategies
typedef enum {
    MOVE_ORDER_CENTER_FIRST,   // Original: 4,5,3,6,2,7,1
    MOVE_ORDER_EDGES_FIRST,    // Try edges first: 1,7,2,6,3,5,4
    MOVE_ORDER_LEFT_TO_RIGHT,  // Simple: 1,2,3,4,5,6,7
    MOVE_ORDER_RIGHT_TO_LEFT   // Simple: 7,6,5,4,3,2,1
} MoveOrderStrategy;

typedef struct {
    int size;
    int data[8];
} MoveOrder;

// Global move ordering configuration
static MoveOrderStrategy current_strategy = MOVE_ORDER_CENTER_FIRST;

// Get move ordering for blue pieces (includes 0 for "no blue piece")
static MoveOrder get_blue_move_order() {
    MoveOrder order;
    switch(current_strategy) {
        case MOVE_ORDER_CENTER_FIRST:
            order.size = 8;
            order.data[0] = 0; order.data[1] = 4; order.data[2] = 5; order.data[3] = 3;
            order.data[4] = 2; order.data[5] = 6; order.data[6] = 1; order.data[7] = 7;
            break;
        case MOVE_ORDER_EDGES_FIRST:
            order.size = 8;
            order.data[0] = 0; order.data[1] = 1; order.data[2] = 7; order.data[3] = 2;
            order.data[4] = 6; order.data[5] = 3; order.data[6] = 5; order.data[7] = 4;
            break;
        case MOVE_ORDER_LEFT_TO_RIGHT:
            order.size = 8;
            for(int i = 0; i < 8; i++) order.data[i] = i;
            break;
        case MOVE_ORDER_RIGHT_TO_LEFT:
            order.size = 8;
            order.data[0] = 0;
            for(int i = 1; i < 8; i++) order.data[i] = 8 - i;
            break;
    }
    return order;
}

// Get move ordering for normal pieces (columns 1-7 only)
static MoveOrder get_normal_move_order() {
    MoveOrder order;
    switch(current_strategy) {
        case MOVE_ORDER_CENTER_FIRST:
            order.size = 7;
            order.data[0] = 4; order.data[1] = 5; order.data[2] = 3;
            order.data[3] = 6; order.data[4] = 2; order.data[5] = 7; order.data[6] = 1;
            break;
        case MOVE_ORDER_EDGES_FIRST:
            order.size = 7;
            order.data[0] = 1; order.data[1] = 7; order.data[2] = 2;
            order.data[3] = 6; order.data[4] = 3; order.data[5] = 5; order.data[6] = 4;
            break;
        case MOVE_ORDER_LEFT_TO_RIGHT:
            order.size = 7;
            for(int i = 0; i < 7; i++) order.data[i] = i + 1;
            break;
        case MOVE_ORDER_RIGHT_TO_LEFT:
            order.size = 7;
            for(int i = 0; i < 7; i++) order.data[i] = 7 - i;
            break;
    }
    return order;
}

// Set the move ordering strategy
void set_move_order_strategy(MoveOrderStrategy strategy) {
    current_strategy = strategy;
}

// Node counter for debugging
static long long node_count = 0;

// Move for PV tracking
typedef struct {
    int blue_col;
    int normal_col;
} Move;

// For mate distance tracking: use scores instead of enum when comparing
// Score = 10000 - depth for wins, -10000 + depth for losses
static inline int result_to_score(C4Result result, int depth) {
    if(result == RESULT_RED) return 10000 - depth;
    if(result == RESULT_YELLOW) return -10000 + depth;
    if(result == RESULT_TIE) return 0;
    return 0; // INCOMPLETE shouldn't happen at terminal nodes
}

static inline C4Result score_to_result(int score) {
    if(score > 5000) return RESULT_RED;
    if(score < -5000) return RESULT_YELLOW;
    return RESULT_TIE;
}

// Internal alpha-beta search with depth limit and PV tracking
static int C4Board_alpha_beta_internal(const C4Board* board, int alpha, int beta, int depth, int max_depth, Move* pv, int* pv_length) {
    node_count++;

    C4Result result = C4Board_who_won(board);
    if(result != RESULT_INCOMPLETE) {
        *pv_length = 0;  // Terminal node, no moves
        return result_to_score(result, depth);
    }

    // Depth limit reached - return incomplete (draw heuristic)
    if(depth >= max_depth) {
        *pv_length = 0;
        return 0; // TIE score
    }

    char illegal_reason[128];
    MoveOrder blue_order = get_blue_move_order();
    MoveOrder normal_order = get_normal_move_order();

    if(board->current_player == 1) {
        // Red's turn (maximizing player - wants high scores)
        int best_score = -100000;
        Move child_pv[100];
        int child_pv_length;
        *pv_length = 0;

        for(int i = 0; i < blue_order.size; i++) {
            int one_index_blue_piece = blue_order.data[i];
            for(int j = 0; j < normal_order.size; j++) {
                int one_index_normal_piece = normal_order.data[j];
                if(C4Board_is_legal(board, one_index_blue_piece, one_index_normal_piece, illegal_reason)) {
                    C4Board child_board = C4Board_child(board, one_index_blue_piece, one_index_normal_piece);
                    int child_score = C4Board_alpha_beta_internal(&child_board, alpha, beta, depth + 1, max_depth, child_pv, &child_pv_length);

                    if(child_score > best_score || *pv_length == 0) {  // Update if better OR no PV yet
                        if(child_score > best_score) best_score = child_score;
                        // Update PV: this move + child's PV
                        pv[0].blue_col = one_index_blue_piece;
                        pv[0].normal_col = one_index_normal_piece;
                        for(int k = 0; k < child_pv_length; k++) {
                            pv[k+1] = child_pv[k];
                        }
                        *pv_length = child_pv_length + 1;
                    }

                    // Standard alpha-beta pruning
                    if(best_score >= beta) return best_score; // Beta cut-off
                    if(alpha < best_score) alpha = best_score;
                }
            }
        }
        return best_score;
    } else {
        // Yellow's turn (minimizing player - wants low scores)
        int best_score = 100000;
        Move child_pv[100];
        int child_pv_length;
        *pv_length = 0;

        for(int i = 0; i < blue_order.size; i++) {
            int one_index_blue_piece = blue_order.data[i];
            for(int j = 0; j < normal_order.size; j++) {
                int one_index_normal_piece = normal_order.data[j];
                if(C4Board_is_legal(board, one_index_blue_piece, one_index_normal_piece, illegal_reason)) {
                    C4Board child_board = C4Board_child(board, one_index_blue_piece, one_index_normal_piece);
                    int child_score = C4Board_alpha_beta_internal(&child_board, alpha, beta, depth + 1, max_depth, child_pv, &child_pv_length);

                    if(child_score < best_score || *pv_length == 0) {  // Update if better OR no PV yet
                        if(child_score < best_score) best_score = child_score;
                        // Update PV: this move + child's PV
                        pv[0].blue_col = one_index_blue_piece;
                        pv[0].normal_col = one_index_normal_piece;
                        for(int k = 0; k < child_pv_length; k++) {
                            pv[k+1] = child_pv[k];
                        }
                        *pv_length = child_pv_length + 1;
                    }

                    // Standard alpha-beta pruning
                    if(best_score <= alpha) return best_score; // Alpha cut-off
                    if(beta > best_score) beta = best_score;
                }
            }
        }
        return best_score;
    }
}

// Public wrapper with iterative deepening
C4Result C4Board_alpha_beta(const C4Board* board, int alpha, int beta) {
    int max_depth = 13;
    Move pv[100];
    int pv_length = 0;
    int score = 0;
    C4Result result = RESULT_INCOMPLETE;

    printf("Starting iterative deepening search (max_depth=%d)...\n", max_depth);

    long long total_nodes = 0;
    node_count = 0;

    // Iterative deepening: search depth 1, 2, 3, ... up to max_depth
    for(int depth = 1; depth <= max_depth; depth++) {
        node_count = 0;
        Move current_pv[100];
        int current_pv_length;

        // Search at current depth
        score = C4Board_alpha_beta_internal(board, alpha, beta, 0, depth, current_pv, &current_pv_length);
        result = score_to_result(score);
        total_nodes += node_count;

        // Update PV for next iteration
        pv_length = current_pv_length;
        for(int i = 0; i < pv_length; i++) {
            pv[i] = current_pv[i];
        }

        printf("  Depth %d: score=%d, nodes=%lld, pv_length=%d\n",
               depth, score, node_count, pv_length);

        // If we found a forced mate within this depth, no need to search deeper
        if(score > 5000 || score < -5000) {
            printf("  Found forced mate at depth %d, stopping search.\n", depth);
            break;
        }
    }

    printf("Search complete! Total nodes: %lld, Score: %d, Result: %s\n",
           total_nodes, score, result_to_string(result));

    // Print principal variation
    if(pv_length > 0) {
        printf("\nPrincipal Variation (%d moves, mate in %d):\n", pv_length, pv_length/2 + 1);
        C4Board sim_board = *board;
        for(int i = 0; i < pv_length; i++) {
            const char* player = C4Board_is_reds_turn(&sim_board) ? "Red" : "Yellow";
            if(pv[i].blue_col == 0) {
                printf("  %d. %s plays: column %d\n", i+1, player, pv[i].normal_col);
            } else {
                printf("  %d. %s plays: blue in column %d, normal in column %d\n",
                       i+1, player, pv[i].blue_col, pv[i].normal_col);
            }
            C4Board_make_move(&sim_board, pv[i].blue_col, pv[i].normal_col);
        }
        printf("\nFinal position:\n");
        C4Board_print(&sim_board);
        printf("Result: %s\n", result_to_string(C4Board_who_won(&sim_board)));
    }

    return result;
}
