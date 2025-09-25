#include "GameUI.hpp"

GameUI::GameUI(SudokuBoard& board) noexcept : board_(board), window_(nullptr) {
    // intialize ncurses
    window_ = initscr();          
    if (!window_) {
        return;
    }
    start_color();                // colors
    keypad(window_, TRUE);        // enabling arrow keys
    noecho();                     // dont echo input
    cbreak();                     // immediate input without Enter
}

GameUI::~GameUI() noexcept {
    if (window_) {
        endwin();                 // Cleanup ncurses
    }
}

void GameUI::displayBoard() const noexcept {

}

void GameUI::showMenu() const noexcept {

}

void GameUI::handleInput() noexcept {

}