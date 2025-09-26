#include "GameUI.hpp"
#include "SudokuBoard.hpp"

int main() {
    SudokuBoard board;
    board.generatePuzzle(SudokuBoard::Difficulty::Easy);
    GameUI ui(board);
    ui.displayBoard();
    mvprintw(SudokuBoard::SIZE * 2 + 3, 0, "Press 'q' to exit.");
    refresh();
    int ch;
    do {
        ch = getch();
    } while (ch != 'q' && ch != 'Q');
    return 0;
}