#include "../solver/alphabeta.c"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

// Red always goes first wlog

void yellow_has_mate_in_2() {       
    C4Board board;
    C4Board_init(&board);
    struct { int first, second; } moves[] = {
        {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 2}, {3, 3}, {0, 3}
    };
    size_t moves_len = sizeof(moves) / sizeof(moves[0]);

    for (size_t i = 0; i < moves_len; ++i) {
        C4Board_make_move(&board, moves[i].first, moves[i].second);
    }
    // Mate sequence starts like this:
    // C4Board_make_move(&board, 0, 2);
    C4Board_print(&board);
    printf("%d\n", C4Board_who_won(&board));
    printf("Red blues remaining: %d\n", board.red_blue_remaining);
    printf("Yellow blues remaining: %d\n", board.yellow_blue_remaining);


    struct timeval start, end;
    gettimeofday(&start, NULL);
    printf("%s\n", result_to_string(C4Board_alpha_beta(&board, -100000, 100000)));
    gettimeofday(&end, NULL);
    long duration = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec);
    printf("Time taken: %ld microseconds\n", duration);
}

void yellow_has_mate_in_5() {
    C4Board board;
    C4Board_init(&board);
    struct { int first, second; } moves[] = {
        {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 3}, {0, 3}, {0, 3}, {0, 4}, {0, 3}, {0, 3}
    };
    size_t moves_len = sizeof(moves) / sizeof(moves[0]);

    for (size_t i = 0; i < moves_len; ++i) {
        C4Board_make_move(&board, moves[i].first, moves[i].second);
    }
    // Mate sequence starts like this:
    // C4Board_make_move(&board, 0, 1);
    // C4Board_make_move(&board, 2, 2);
    // C4Board_make_move(&board, 0, 5);
    // C4Board_make_move(&board, 2, 2);
    // C4Board_make_move(&board, 0, 7);
    C4Board_print(&board);
    printf("%d\n", C4Board_who_won(&board));
    printf("Red blues remaining: %d\n", board.red_blue_remaining);
    printf("Yellow blues remaining: %d\n", board.yellow_blue_remaining);


    struct timeval start, end;
    gettimeofday(&start, NULL);
    printf("%s\n", result_to_string(C4Board_alpha_beta(&board, -100000, 100000)));
    gettimeofday(&end, NULL);
    long duration = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec);
    printf("Time taken: %ld microseconds\n", duration);
}

void yellow_has_mate_in_3() {
    C4Board board;
    C4Board_init(&board);
    struct { int first, second; } moves[] = {
        {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 3}, {0, 3}, {0, 3}, {0, 4}, {0, 1}, {2, 2}, {0, 5}, {2, 2}, {0, 3}, {0, 3}, {0, 3}, {0, 2}
    };
    size_t moves_len = sizeof(moves) / sizeof(moves[0]);

    for (size_t i = 0; i < moves_len; ++i) {
        C4Board_make_move(&board, moves[i].first, moves[i].second);
    }
    // Mate sequence starts like this:
    // C4Board_make_move(&board, 0, 1);
    // C4Board_make_move(&board, 2, 2);
    // C4Board_make_move(&board, 0, 5);
    // C4Board_make_move(&board, 2, 2);
    // C4Board_make_move(&board, 0, 7);
    C4Board_print(&board);
    printf("%d\n", C4Board_who_won(&board));
    printf("Red blues remaining: %d\n", board.red_blue_remaining);
    printf("Yellow blues remaining: %d\n", board.yellow_blue_remaining);


    struct timeval start, end;
    gettimeofday(&start, NULL);
    printf("%s\n", result_to_string(C4Board_alpha_beta(&board, -100000, 100000)));
    gettimeofday(&end, NULL);
    long duration = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec);
    printf("Time taken: %ld microseconds\n", duration);
}

void yellow_has_mate_in() {
    C4Board board;
    C4Board_init(&board);
    struct { int first, second; } moves[] = {
        {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}
    };
    size_t moves_len = sizeof(moves) / sizeof(moves[0]);

    for (size_t i = 0; i < moves_len; ++i) {
        C4Board_make_move(&board, moves[i].first, moves[i].second);
    }
    // Mate sequence starts like this:
    // C4Board_make_move(&board, 0, 1);
    // C4Board_make_move(&board, 2, 2);
    // C4Board_make_move(&board, 0, 5);
    // C4Board_make_move(&board, 2, 2);
    // C4Board_make_move(&board, 0, 7);
    C4Board_print(&board);
    printf("%d\n", C4Board_who_won(&board));
    printf("Red blues remaining: %d\n", board.red_blue_remaining);
    printf("Yellow blues remaining: %d\n", board.yellow_blue_remaining);


    struct timeval start, end;
    gettimeofday(&start, NULL);
    printf("%s\n", result_to_string(C4Board_alpha_beta(&board, -100000, 100000)));
    gettimeofday(&end, NULL);
    long duration = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec);
    printf("Time taken: %ld microseconds\n", duration);
}

int main() {
    //yellow_has_mate_in_2();
    //yellow_has_mate_in_3();
    yellow_has_mate_in();
    return 0;
}
