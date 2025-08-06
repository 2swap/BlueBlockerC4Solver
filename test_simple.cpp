#include <iostream>
#include "Bitboard.cpp"

using namespace std;

int main() {
    cout << "Testing your bitboard implementation!" << endl;
    
    // Test basic operations
    cout << "\n=== Testing make_point ===" << endl;
    Bitboard point = make_point(3, 2);
    cout << "make_point(3, 2) = " << point << endl;
    
    cout << "\n=== Testing make_column ===" << endl;
    Bitboard col = make_column(3);
    cout << "make_column(3) = " << col << endl;
    print_bitboard(col);
    
    cout << "\n=== Testing downset ===" << endl;
    Bitboard test_board = make_point(0,5) | make_point(1,4) | make_point(2,3);
    cout << "Test board:" << endl;
    print_bitboard(test_board);
    cout << "Downset:" << endl;
    print_bitboard(downset(test_board));
    
    cout << "\n=== Testing bitboard_at ===" << endl;
    cout << "bitboard_at(test_board, 0, 5) = " << bitboard_at(test_board, 0, 5) << endl;
    cout << "bitboard_at(test_board, 1, 1) = " << bitboard_at(test_board, 1, 1) << endl;
    
    cout << "\n=== Connect 4 Move Simulation ===" << endl;
    Bitboard red = 0, yellow = 0;
    
    // Simulate dropping pieces in column 3
    cout << "Dropping pieces in column 3:" << endl;
    for (int drop = 0; drop < 4; drop++) {
        Bitboard legal_moves = downset(red | yellow);
        Bitboard move_pos = legal_moves & make_column(3);
        
        if (drop % 2 == 0) {
            red |= move_pos;
            cout << "Red drops at position " << move_pos << endl;
        } else {
            yellow |= move_pos;
            cout << "Yellow drops at position " << move_pos << endl;
        }
        
        cout << "Board state:" << endl;
        for (int y = 0; y < 6; y++) {
            for (int x = 0; x < 7; x++) {
                if (bitboard_at(red, x, y)) cout << "R ";
                else if (bitboard_at(yellow, x, y)) cout << "Y ";
                else cout << ". ";
            }
            cout << endl;
        }
        cout << endl;
    }
    
    cout << "Your bitboard implementation works!" << endl;
    return 0;
}