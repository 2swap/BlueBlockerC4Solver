#include "C4Board.cpp"

/*
int main() {
    //cout << "Running existing unit tests..." << endl;
    //c4_unit_tests();
    
    cout << "\nTesting blue blocker functionality..." << endl;
    
    // Test basic blue piece functionality
    C4Board board;
    cout << "Initial state - Red's turn: " << board.is_reds_turn() << endl;
    cout << "Red blue remaining: " << board.get_blue_remaining() << endl;
    
    // Play a regular piece
    board.play_piece(4);
    cout << "After regular move - Red's turn: " << board.is_reds_turn() << endl;
    
    // Play a blue piece (should keep it yellow's turn)
    board.play_piece(3, BLUE);
    cout << "After blue move - Red's turn: " << board.is_reds_turn() << endl;
    cout << "Yellow blue remaining: " << board.get_blue_remaining() << endl;
    
    // Play another yellow piece
    board.play_piece(5);
    cout << "After second yellow move - Red's turn: " << board.is_reds_turn() << endl;
    
    board.print();
    cout << "Representation: " << board.representation << endl;
    
    return 0;
}
*/

int main() {
	C4Board board;
	board.make_move(0, 4);
	board.make_move(0, 4);
	board.make_move(0, 4);
	board.make_move(0, 4);
	board.make_move(0, 4);
	board.make_move(0, 4);
	board.make_move(0, 2);
	board.make_move(0, 3);
	board.make_move(0, 2);
	board.make_move(0, 2);
	board.make_move(2, 2);
	board.make_move(0, 2);
	board.make_move(0, 3);
	cout << board.who_won()<< endl;
	board.make_move(0, 7);
	board.make_move(0, 5);
	board.make_move(6, 6);
	board.make_move(5, 5);
	board.make_move(0, 1);
	board.make_move(0, 1);
	board.make_move(0, 1);
	board.make_move(0, 1);
	board.make_move(0, 1);
	board.make_move(0, 1);
	board.make_move(0, 5);
	board.make_move(0, 3);
	board.make_move(0, 3);
	board.make_move(0, 3);
	board.make_move(0, 3);
	board.make_move(0, 7);
	board.make_move(0, 5);
	board.make_move(0, 5);
	board.make_move(0, 6);
	board.make_move(0, 7);
	board.make_move(7, 7);
	board.print();
	cout << "Representation: " << board.representation << endl;
	cout << board.who_won() << endl;
}
