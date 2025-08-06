#pragma once

#include "C4Board.h"
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>

C4Board::C4Board() { }

C4Board::C4Board(const string& rep) {
    fill_board_from_string(rep);
}

int C4Board::piece_code_at(int x, int y) const {
    return bitboard_at(red_bitboard, x, y) + (2*bitboard_at(yellow_bitboard, x, y)) + (3*bitboard_at(blue_bitboard, x, y));
}
void C4Board::print() const {
    cout << representation << endl;
    for(int y = 0; y < C4_HEIGHT; y++) {
        for(int x = 0; x < C4_WIDTH; x++) {
            cout << disk_col(piece_code_at(x, y)) << " ";
        }
        cout << endl;
    }
}

Bitboard C4Board::legal_moves() const {
    return downset(red_bitboard | yellow_bitboard | blue_bitboard);
}

bool C4Board::is_legal(int x) const {
    return (((red_bitboard|yellow_bitboard|blue_bitboard) >> (x-1)) & 1ul) == 0ul;
}

const Bitboard full_bitboard = 140185576636287ul;
C4Result C4Board::who_won() const {
    const int v = C4_WIDTH;
    const int w = C4_WIDTH + 1; // there is a space of padding on the right of the bitboard
    const int x = C4_WIDTH + 2; // since otherwise horizontal wins would wrap walls
    if((yellow_bitboard|red_bitboard|blue_bitboard) == full_bitboard)
        return TIE;

    for (int i = 0; i < 2; i++){
        const Bitboard b = i==0?red_bitboard:yellow_bitboard;
        if( (b & (b>>1) & (b>>(2*1)) & (b>>(3*1)))
         || (b & (b>>w) & (b>>(2*w)) & (b>>(3*w)))
         || (b & (b>>v) & (b>>(2*v)) & (b>>(3*v)))
         || (b & (b>>x) & (b>>(2*x)) & (b>>(3*x))) )
            return i==0?RED:YELLOW;
    }

    return INCOMPLETE;
}

bool C4Board::is_solution() {
    C4Result winner = who_won();
    return winner == RED || winner == YELLOW;
}

void C4Board::fill_board_from_string(const string& rep) {
    // Iterate through the moves and fill the board
    for (int i = 0; i < rep.size(); i++) {
        play_piece(rep[i]-'0');
    }
}

void C4Board::play_piece(int piece){
    play_piece(piece, REGULAR);
}

void C4Board::play_piece(int piece, PieceType type){
    if(piece < 0) {
        print();
        throw runtime_error("Attempted playing a piece in an illegal column. Representation: " + representation + ", piece: " + to_string(piece));
    }
    if(piece > 0){
        if(!is_legal(piece)) {
            print();
            throw runtime_error("Tried playing illegal piece " + representation + " " + to_string(piece));
        }
        if(type == BLUE && !can_play_blue()) {
            print();
            throw runtime_error("Attempted to play blue piece when none remaining");
        }
        if(type == BLUE && last_move_was_blue) {
            print();
            throw runtime_error("Cannot play consecutive blue pieces - must play regular piece after blue");
        }
        
        int x = piece - 1; // convert from 1index to 0
        Bitboard p = legal_moves() & make_column(x);
        
        if(type == BLUE) {
            blue_bitboard += p;
            if(current_player == 1) red_blue_remaining--;
            else yellow_blue_remaining--;
            last_move_was_blue = true;
            // Don't switch turns after blue piece
        } else {
            if(current_player == 1) red_bitboard += p;
            else yellow_bitboard += p;
            last_move_was_blue = false;
            // Switch turns after regular piece
            current_player = (current_player == 1) ? 2 : 1;
        }
    }
    representation += to_string(piece);
    if(type == BLUE) representation += "b";
}

C4Board C4Board::child(int piece) const{
    C4Board new_board(*this);
    new_board.play_piece(piece);
    return new_board;
}

C4Board C4Board::child(int piece, PieceType type) const{
    C4Board new_board(*this);
    new_board.play_piece(piece, type);
    return new_board;
}

bool C4Board::can_play_blue() const{
    if(current_player == 1) return red_blue_remaining > 0;
    else return yellow_blue_remaining > 0;
}

int C4Board::get_blue_remaining() const{
    if(current_player == 1) return red_blue_remaining;
    else return yellow_blue_remaining;
}

int C4Board::get_instant_win() const{
    for (int x = 1; x <= C4_WIDTH; ++x){
        if(!is_legal(x)) continue;
        C4Result whowon = child(x).who_won();
        if(whowon == RED || whowon == YELLOW)
            return x;
    }
    return -1;
}

int C4Board::get_blocking_move() const{
    return child(0).get_instant_win();
}

bool C4Board::is_reds_turn() const{
    return current_player == 1;
}
