#include "C4Board.cpp"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

void display_game_state(const C4Board& board) {
    cout << "\n=== Current Game State ===" << endl;
    board.print();
    cout << "\nCurrent player: " << (board.is_reds_turn() ? "Red" : "Yellow") << endl;
    cout << "Red blue pieces remaining: " << (board.is_reds_turn() ? board.get_blue_remaining() : board.red_blue_remaining) << endl;
    cout << "Yellow blue pieces remaining: " << (!board.is_reds_turn() ? board.get_blue_remaining() : board.yellow_blue_remaining) << endl;
    
    C4Result winner = board.who_won();
    if (winner == RED) {
        cout << "*** RED WINS! ***" << endl;
    } else if (winner == YELLOW) {
        cout << "*** YELLOW WINS! ***" << endl;
    } else if (winner == TIE) {
        cout << "*** GAME IS A TIE! ***" << endl;
    } else {
        cout << "Game continues..." << endl;
    }
    cout << "=========================" << endl;
}

void show_help() {
    cout << "\nCommands:" << endl;
    cout << "  1-7: Play a regular piece in column 1-7" << endl;
    cout << "  b1-b7: Play a blue piece in column 1-7" << endl;
    cout << "  help: Show this help message" << endl;
    cout << "  quit: Exit the game" << endl;
    cout << "  print: Show current board state" << endl;
}

int main() {
    C4Board board;
    string input;
    
    cout << "Welcome to Interactive Connect 4!" << endl;
    cout << "Type 'help' for commands." << endl;
    
    display_game_state(board);
    
    while (true) {
        C4Result winner = board.who_won();
        if (winner != INCOMPLETE) {
            cout << "\nGame Over! Type 'quit' to exit or any other key to start a new game." << endl;
            getline(cin, input);
            if (input == "quit") {
                break;
            } else {
                board = C4Board(); // Reset the board
                display_game_state(board);
                continue;
            }
        }
        
        cout << "\nEnter your move: ";
        getline(cin, input);
        
        // Convert to lowercase for easier parsing
        transform(input.begin(), input.end(), input.begin(), ::tolower);
        
        if (input == "quit") {
            break;
        } else if (input == "help") {
            show_help();
            continue;
        } else if (input == "print") {
            display_game_state(board);
            continue;
        }
        
        try {
            bool is_blue = false;
            int column = 0;
            
            // Parse blue piece moves (b1, b2, etc.)
            if (input.length() == 2 && input[0] == 'b' && input[1] >= '1' && input[1] <= '7') {
                is_blue = true;
                column = input[1] - '0';
            }
            // Parse regular moves (1, 2, etc.)
            else if (input.length() == 1 && input[0] >= '1' && input[0] <= '7') {
                column = input[0] - '0';
            } else {
                cout << "Invalid input. Type 'help' for valid commands." << endl;
                continue;
            }
            
            // Validate the move
            if (!board.is_legal(column)) {
                cout << "Column " << column << " is full! Choose another column." << endl;
                continue;
            }
            
            if (is_blue && !board.can_play_blue()) {
                cout << "No blue pieces remaining for current player!" << endl;
                continue;
            }
            
            // Make the move
            if (is_blue) {
                board.play_piece(column, BLUE);
            } else {
                board.play_piece(column, REGULAR);
            }
            
            display_game_state(board);
            
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            cout << "Please try again." << endl;
        }
    }
    
    cout << "Thanks for playing!" << endl;
    return 0;
}
