#include "C4Board.cpp"
#include <vector>
#include <utility>
#include <iostream>
#include <chrono>
using namespace std;

int deprecated_game_log() {
    C4Board board;
    vector<pair<int,int>> moves = {
        {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4},
        {0, 2}, {0, 3}, {0, 2}, {0, 2}, {2, 2}, {0, 2},
        {0, 3}, {0, 7}, {0, 5}, {6, 6}, {5, 5}, {0, 1},
        {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 5},
        {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 7}, {0, 5},
        {0, 5}, {0, 6}, {0, 7}, {7, 7}
    };

    for (size_t i = 0; i < moves.size(); ++i) {
        board.make_move(moves[i].first, moves[i].second);
        board.print();
        // Time the alpha-beta search
        if(i > 17) {
            static auto start = chrono::high_resolution_clock::now();
            cout << result_to_string(board.alpha_beta(-100, 100)) << endl;
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();
            cout << "Time taken: " << duration << " microseconds" << endl;
        }
        cout << endl << endl;
    }
}

void yellow_has_mate_in_2() {       
    C4Board board;
    vector<pair<int,int>> moves = {
        {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 2}, {3, 3}, {0, 3}
    };

    for (size_t i = 0; i < moves.size(); ++i) {
        board.make_move(moves[i].first, moves[i].second);
    }
    // Mate sequence starts like this:
    // board.make_move(0, 2);
    board.print();
    cout << "Representation: " << board.representation << endl;
    cout << board.who_won() << endl;

    static auto start = chrono::high_resolution_clock::now();
    cout << result_to_string(board.alpha_beta(-100, 100)) << endl;
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();
    cout << "Time taken: " << duration << " microseconds" << endl;
}

void yellow_has_mate_in_8() {
    C4Board board;
    vector<pair<int,int>> moves = {
        {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 3}, {0, 3}, {0, 3}, {0, 4}
    };

    for (size_t i = 0; i < moves.size(); ++i) {
        board.make_move(moves[i].first, moves[i].second);
    }
    // Mate sequence starts like this:
    // board.make_move(0, 1);
    // board.make_move(2, 2);
    // board.make_move(0, 5);
    // board.make_move(2, 2);
    // board.make_move(0, 7);
    board.print();
    cout << "Representation: " << board.representation << endl;
    cout << board.who_won() << endl;

    static auto start = chrono::high_resolution_clock::now();
    cout << result_to_string(board.alpha_beta(-100, 100)) << endl;
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();
    cout << "Time taken: " << duration << " microseconds" << endl;
}

int main() {
    yellow_has_mate_in_2();
    yellow_has_mate_in_8();
}
