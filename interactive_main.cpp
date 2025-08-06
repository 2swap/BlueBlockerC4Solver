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
    if (winner == RESULT_RED) {
        cout << "*** RED WINS! ***" << endl;
    } else if (winner == RESULT_YELLOW) {
        cout << "*** YELLOW WINS! ***" << endl;
    } else if (winner == RESULT_TIE) {
        cout << "*** GAME IS A TIE! ***" << endl;
    } else {
        cout << "Game continues..." << endl;
    }
    cout << "=========================" << endl;
}

void show_help() {
    cout << "\nCommands:" << endl;
    cout << "  1-7: Play a regular piece in column 1-7" << endl;
    cout << "  1b 2: Play a blue piece in column 1, then a normal piece in column 2" << endl;
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
        if (winner != RESULT_INCOMPLETE) {
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
            int one_index_blue_column = 0;
            int one_index_normal_column = 0;
            
            // Parse blue piece moves (b1, b2, etc.)
            if (input.length() == 4 && input[1] == 'b' && input[2] == ' ' && input[0] >= '1' && input[0] <= '7' && input[3] >= '1' && input[3] <= '7') {
                one_index_blue_column = input[0] - '0';
                one_index_normal_column = input[3] - '0';
            }
            // Parse regular moves (1, 2, etc.)
            else if (input.length() == 1 && input[0] >= '1' && input[0] <= '7') {
                one_index_normal_column = input[0] - '0';
            } else {
                cout << "Invalid input. Type 'help' for valid commands." << endl;
                continue;
            }
            
            cout << "Processing move with blue column: " << one_index_blue_column << " and normal column: " << one_index_normal_column << endl;
            string illegal_reason;
            if(!board.is_legal(one_index_blue_column, one_index_normal_column, illegal_reason)) {
                cout << "Illegal move: " << illegal_reason << endl;
                continue;
            }
            
            board.make_move(one_index_blue_column, one_index_normal_column);

            display_game_state(board);
            
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            cout << "Please try again." << endl;
        }
    }
    
    cout << "Thanks for playing!" << endl;
    return 0;
}
