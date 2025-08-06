#include "C4Board.c"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

struct moveset {
    int first, second;
};

void run_test(const struct moveset moves[], size_t moves_len, int alpha, int beta) {
    C4Board board;
    C4Board_init(&board);

    for (size_t i = 0; i < moves_len; ++i) {
        C4Board_make_move(&board, moves[i].first, moves[i].second);
    }

    C4Board_print(&board);
    printf("%d\n", C4Board_who_won(&board));

    struct timeval start, end;
    gettimeofday(&start, NULL);
    printf("%s\n", result_to_string(C4Board_alpha_beta(&board, alpha, beta)));
    gettimeofday(&end, NULL);
    long duration = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec);
    printf("Time taken: %ld microseconds\n", duration);
}

void easy() {
    struct moveset moves[] = {
        {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 2}, {3, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 4}, {0, 4}, {0, 1},
        {0, 5}, {0, 5}, {0, 5}, {0, 5}
    };
    size_t moves_len = sizeof(moves) / sizeof(moves[0]);
    run_test(moves, moves_len, -43, -42);
    run_test(moves, moves_len, 42, 43);
}

void yellow_has_mate_in_2() {
    struct moveset moves[] = {
        {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 2}, {3, 3}, {0, 3}
    };
    size_t moves_len = sizeof(moves) / sizeof(moves[0]);
    run_test(moves, moves_len, -14, 14);
}

void yellow_has_mate_in_8() {
    struct moveset moves[] = {
        {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 3}, {0, 3}, {0, 3}, {0, 4}
    };
    size_t moves_len = sizeof(moves) / sizeof(moves[0]);
    run_test(moves, moves_len, -100, 100);
}

int main() {
    easy();
    yellow_has_mate_in_2();
    yellow_has_mate_in_8();
    return 0;
}
