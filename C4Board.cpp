#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <cassert>
#include <iostream>
#include <list>
#include "Bitboard.cpp"

const int BOARD_HEIGHT = 6;
const int BOARD_WIDTH = 7;
const Bitboard full_bitboard = 140185576636287ul;

// Note: these are ordered according to Red's heuristic value for alpha-beta pruning.
enum C4Result {
    RESULT_YELLOW,
    RESULT_TIE,
    RESULT_RED,
    RESULT_INCOMPLETE
};

enum PieceType {
    PIECETYPE_RED = 0,
    PIECETYPE_YELLOW = 1,
    PIECETYPE_BLUE = 2
};

string result_to_string(C4Result r) {
    switch(r) {
        case RESULT_YELLOW: return "YELLOW";
        case RESULT_TIE: return "TIE";
        case RESULT_RED: return "RED";
        case RESULT_INCOMPLETE: return "INCOMPLETE";
    }
    return "UNKNOWN";
}

string disk_col(int i){
    if(i == 1) return "\033[31mx\033[0m";  // Red "x"
    if(i == 2) return "\033[33mo\033[0m";  // Yellow "o"
    if(i == 3) return "\033[34mB\033[0m";  // Blue "B"
    return ".";
}

class C4Board {
public:
    Bitboard red_bitboard = 0ul, yellow_bitboard = 0ul, blue_bitboard = 0ul;
    int red_blue_remaining = 2, yellow_blue_remaining = 2;
    int current_player = 1; // 1 for red, 2 for yellow
    //string representation = "";

    C4Board() { }

    int piece_code_at(int x, int y) const {
        return bitboard_at(red_bitboard, x, y) + (2*bitboard_at(yellow_bitboard, x, y)) + (3*bitboard_at(blue_bitboard, x, y));
    }
    void print() const {
        //cout << representation << endl;
        for(int y = 0; y < BOARD_HEIGHT; y++) {
            for(int x = 0; x < BOARD_WIDTH; x++) {
                cout << disk_col(piece_code_at(x, y)) << " ";
            }
            cout << endl;
        }
    }

    Bitboard legal_moves() const {
        return downset(red_bitboard | yellow_bitboard | blue_bitboard);
    }

    // Check if a column is full
    bool column_is_full(int one_index_column) const {
        return (((red_bitboard|yellow_bitboard|blue_bitboard) >> (one_index_column-1)) & 1ul) != 0ul;
    }

    C4Result who_won() const {
        const int v = BOARD_WIDTH;
        const int w = BOARD_WIDTH + 1; // there is a space of padding on the right of the bitboard
        const int x = BOARD_WIDTH + 2; // since otherwise horizontal wins would wrap walls
        if((yellow_bitboard|red_bitboard|blue_bitboard) == full_bitboard)
            return RESULT_TIE;

        for (int i = 0; i < 2; i++){
            const Bitboard b = i==0?red_bitboard:yellow_bitboard;
            if( (b & (b>>1) & (b>>(2*1)) & (b>>(3*1)))
             || (b & (b>>w) & (b>>(2*w)) & (b>>(3*w)))
             || (b & (b>>v) & (b>>(2*v)) & (b>>(3*v)))
             || (b & (b>>x) & (b>>(2*x)) & (b>>(3*x))) )
                return i==0?RESULT_RED:RESULT_YELLOW;
        }

        return RESULT_INCOMPLETE;
    }

    bool is_solution() {
        C4Result winner = who_won();
        return winner == RESULT_RED || winner == RESULT_YELLOW;
    }

    bool is_legal(int one_index_blue_piece, int one_index_normal_piece, string& illegal_reason) const {
        // A move consists of an optional blue piece and a required normal piece.
        // A blue piece value of 0 means no blue piece is played.

        // Do the blue piece logic
        if(one_index_blue_piece < 0 || one_index_blue_piece > BOARD_WIDTH) {
            illegal_reason = "Blue piece in illegal column (" + to_string(one_index_blue_piece) + ")";
            return false;
        }
        if(one_index_blue_piece != 0) {
            if(column_is_full(one_index_blue_piece)) {
                illegal_reason = "Tried playing blue piece in full column";
                return false;
            }
            if(!can_play_blue()) {
                illegal_reason = "Attempted to play blue piece when none remaining for current player";
                return false;
            }
            C4Board simulated_board(*this);
            simulated_board.play_piece(PIECETYPE_BLUE, one_index_blue_piece);
        }

        // Normal piece logic
        if(one_index_normal_piece < 1 || one_index_normal_piece > BOARD_WIDTH) {
            illegal_reason = "Normal piece in illegal column";
            return false;
        }
        if(column_is_full(one_index_normal_piece)) {
            illegal_reason = "Tried playing illegal normal piece in full column";
            return false;
        }

        illegal_reason = "Move is legal";
        return true;
    }

    void make_move(int one_index_blue_piece, int one_index_normal_piece) {
        // A move consists of an optional blue piece and a required normal piece.
        // A blue piece value of 0 means no blue piece is played.

        // Do the blue piece logic
        string illegal_reason = "";
        bool check = is_legal(one_index_blue_piece, one_index_normal_piece, illegal_reason);
        if(!check) {
            print();
            throw runtime_error("Attempted to play illegal move: " + illegal_reason + /*". Representation: " + representation*/ + ", blue piece: " + to_string(one_index_blue_piece) + ", normal piece: " + to_string(one_index_normal_piece));
        }

        if(one_index_blue_piece != 0) {
            play_piece(PIECETYPE_BLUE, one_index_blue_piece);
        }
        play_piece(current_player == 1 ? PIECETYPE_RED : PIECETYPE_YELLOW, one_index_normal_piece);
    }

    C4Board child(int one_index_blue_piece, int one_index_normal_piece) const{
        C4Board new_board(*this);
        new_board.make_move(one_index_blue_piece, one_index_normal_piece);
        return new_board;
    }

    bool can_play_blue() const{
        if(current_player == 1) return red_blue_remaining > 0;
        else return yellow_blue_remaining > 0;
    }

    int get_blue_remaining() const{
        if(current_player == 1) return red_blue_remaining;
        else return yellow_blue_remaining;
    }

    bool is_reds_turn() const{
        return current_player == 1;
    }

    vector<int> naive_move_ordering_blue = {0,4,5,3,2,6,1,7};
    vector<int> naive_move_ordering_normal = {4,5,3,2,6,1,7};
    C4Result alpha_beta() const {
        return alpha_beta(0, 2);
    }
    C4Result alpha_beta(int alpha, int beta) const {
        C4Result result = who_won();
        if(result != RESULT_INCOMPLETE) return result;
        string illegal_reason = "";
        if(current_player == 1) {
            C4Result best_result = RESULT_YELLOW;
            for(int one_index_blue_piece : naive_move_ordering_blue) {
                for(int one_index_normal_piece : naive_move_ordering_normal) {
                    if(is_legal(one_index_blue_piece, one_index_normal_piece, illegal_reason)) {
                        C4Board child_board = child(one_index_blue_piece, one_index_normal_piece);
                        C4Result child_result = child_board.alpha_beta(alpha, beta);
                        if(child_result == RESULT_RED) return RESULT_RED; // Red wins immediately
                        if(child_result > best_result) best_result = child_result;
                        if(best_result >= beta) break; // Beta cut-off
                        alpha = max(alpha, static_cast<int>(best_result));
                    }
                }
            }
            return best_result;
        } else {
            C4Result best_result = RESULT_RED;
            for(int one_index_blue_piece : naive_move_ordering_blue) {
                for(int one_index_normal_piece : naive_move_ordering_normal) {
                    if(is_legal(one_index_blue_piece, one_index_normal_piece, illegal_reason)) {
                        C4Board child_board = child(one_index_blue_piece, one_index_normal_piece);
                        C4Result child_result = child_board.alpha_beta(alpha, beta);
                        if(child_result == RESULT_YELLOW) return RESULT_YELLOW; // Yellow wins immediately
                        if(child_result < best_result) best_result = child_result;
                        if(best_result <= alpha) break; // Alpha cut-off
                        beta = min(beta, static_cast<int>(best_result));
                    }
                }
            }
            return best_result;
        }
    }

private:
    // This function merely modifies the bitboards and board representation, and does not orchestrate move ordering.
    // For speed, we do no error checking here. Caller should check the move is legal.
    void play_piece(PieceType pt, int one_index_column) {
        Bitboard p = legal_moves() & make_column(one_index_column-1);
        if(pt != PIECETYPE_BLUE) current_player = (current_player == 1) ? 2 : 1;
        switch(pt) {
            case PIECETYPE_RED:
                red_bitboard |= p;
                break;
            case PIECETYPE_YELLOW:
                yellow_bitboard |= p;
                break;
            case PIECETYPE_BLUE:
                blue_bitboard |= p;
                //representation += 'b';
                break;
        }
        //representation += to_string(one_index_column);
    }
};
