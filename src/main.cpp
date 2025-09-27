#include "GameUI.hpp"
#include "SudokuBoard.hpp"

int main() {
    SudokuBoard board;
    board.generatePuzzle(SudokuBoard::Difficulty::Easy);
    GameUI ui(board);

    ui.displayBoard(); 

    while (ui.handleInput()) {
        // Loop body can be empty as all handling is done in handleInput
    }
    
    return 0;
}