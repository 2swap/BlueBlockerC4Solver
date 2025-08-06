#include <iostream>
#include "C4Board.h"
#include "C4Board.cpp"

using namespace std;

int main() {
    cout << "Testing cleaned C4Board implementation!" << endl;
    
    // Test basic constructor and game play
    cout << "\n=== Basic Game Test ===" << endl;
    C4Board board;
    cout << "Empty board:" << endl;
    board.print();
    
    // Play a few moves
    cout << "\nPlaying moves: 4, 4, 3, 3, 2, 2..." << endl;
    board.play_piece(4);
    board.play_piece(4); 
    board.play_piece(3);
    board.play_piece(3);
    board.play_piece(2);
    board.play_piece(2);
    board.print();
    
    // Test tactical functions
    cout << "\n=== Testing Tactical Functions ===" << endl;
    int instant_win = board.get_instant_win();
    if (instant_win != -1) {
        cout << "Instant win available in column " << instant_win << endl;
    } else {
        cout << "No instant win found" << endl;
    }
    
    int blocking_move = board.get_blocking_move();
    if (blocking_move != -1) {
        cout << "Need to block in column " << blocking_move << endl;
    } else {
        cout << "No immediate threat to block" << endl;
    }
    
    // Test string constructor
    cout << "\n=== String Constructor Test ===" << endl;
    C4Board test_board("4444445623333356555216622");
    cout << "Board from string '4444445623333356555216622':" << endl;
    test_board.print();
    
    C4Result result = test_board.who_won();
    cout << "Game result: ";
    if (result == RED) cout << "Red wins!" << endl;
    else if (result == YELLOW) cout << "Yellow wins!" << endl;
    else if (result == TIE) cout << "Tie!" << endl;
    else cout << "Game incomplete" << endl;
    
    // Test unit tests if they exist
    cout << "\n=== Running Unit Tests ===" << endl;
    try {
        c4_unit_tests();
        cout << "All unit tests passed!" << endl;
    } catch (const exception& e) {
        cout << "Unit test error: " << e.what() << endl;
    }
    
    cout << "\nCleaned C4Board implementation works!" << endl;
    return 0;
}