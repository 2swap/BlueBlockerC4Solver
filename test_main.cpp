#include "C4Board.cpp"
#include <vector>
#include <utility>
#include <iostream>
#include <chrono>
using namespace std;

int main() {
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
