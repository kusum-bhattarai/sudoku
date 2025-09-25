#include "GameUI.hpp"
#include "SudokuBoard.hpp"

int main() {
    SudokuBoard board;
    GameUI ui(board);
    // Test ncurses setup: display a message and wait for keypress
    mvprintw(0, 0, "ncurses initialized. Press any key to exit.");
    refresh();
    getch();
    return 0;
}