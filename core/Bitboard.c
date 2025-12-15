#pragma once

#include <stdio.h>

typedef unsigned long int Bitboard;

// ALL BITBOARDS ARE 8x8.
static const int BITBOARD_WIDTH = 8;
static const int BITBOARD_HEIGHT = 8;
const unsigned long frame = 0b1111111111111111100000001000000010000000100000001000000010000000UL;

static inline Bitboard make_point(int x, int y){
    return (1ul << x) << (y*BITBOARD_WIDTH);
}

static inline int bitboard_at(Bitboard b, int x, int y){
    return (b >> x >> y*BITBOARD_WIDTH) & 1ul;
}

static inline Bitboard downset(Bitboard b1) {
    Bitboard downshift = (b1 | frame) >> BITBOARD_WIDTH;
    return (downshift ^ (b1 | frame)) & ~frame;
}

static inline Bitboard make_column(int column_number) {
    const unsigned long left_column = 0b0000000100000001000000010000000100000001000000010000000100000001UL;
    return left_column << column_number; 
}

static inline void print_bitboard(Bitboard b) {
    for (int y = 0; y < BITBOARD_HEIGHT; ++y) {
        for (int x = 0; x < BITBOARD_WIDTH; ++x) {
            printf("%lu", (unsigned long)bitboard_at(b, x, y));
        }
        printf("\n");
    }
    printf("\n");
}

static inline int is_power_of_two(Bitboard b) {
    return 0 == (b & (b-1));
}

static const Bitboard even_rows = 0b0000000011111111000000001111111100000000111111110000000011111111UL;
static const Bitboard  odd_rows = 0b1111111100000000111111110000000011111111000000001111111100000000UL;
