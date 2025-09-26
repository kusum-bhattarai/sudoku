#include "GameUI.hpp"

GameUI::GameUI(SudokuBoard& board) noexcept : board_(board), window_(nullptr) {
    window_ = initscr();
    if (!window_) {
        return;
    }
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_BLUE, COLOR_BLACK);
    }
    keypad(window_, TRUE);
    noecho();
    cbreak();
    curs_set(0); // Hide cursor
}

GameUI::~GameUI() noexcept {
    if (window_) {
        endwin();                 // Cleanup ncurses
    }
}

void GameUI::displayBoard() const noexcept {
    if (!window_) return;
    werase(window_); // Use window-specific clear

    constexpr int CELL_WIDTH = 5; // | 5 | (3 spaces + borders)
    constexpr int GRID_WIDTH = SudokuBoard::SIZE * CELL_WIDTH + 1;
    constexpr int GRID_HEIGHT = SudokuBoard::SIZE * 2 + 1;
    constexpr int LABEL_OFFSET = 2; // Space for labels (A-I, 1-9)

    // Draw column labels (A-I)
    for (int col = 0; col < SudokuBoard::SIZE; ++col) {
        mvwaddch(window_, LABEL_OFFSET - 1, col * CELL_WIDTH + 3, 'A' + col);
    }

    // Draw row labels (1-9)
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        mvwaddch(window_, row * 2 + LABEL_OFFSET + 1, 0, '1' + row);
    }

    // Draw vertical lines
    for (int i = 0; i <= SudokuBoard::SIZE; ++i) {
        int x = i * CELL_WIDTH + LABEL_OFFSET;
        char vchar = (i % 3 == 0) ? '#' : '|';
        mvwvline(window_, LABEL_OFFSET, x, vchar, GRID_HEIGHT);
    }

    // Draw horizontal lines
    for (int i = 0; i <= SudokuBoard::SIZE; ++i) {
        int y = i * 2 + LABEL_OFFSET;
        char hchar = (i % 3 == 0) ? '=' : '-';
        mvwhline(window_, y, LABEL_OFFSET, hchar, GRID_WIDTH);
    }

    // Draw intersections
    for (int i = 0; i <= SudokuBoard::SIZE; ++i) {
        for (int j = 0; j <= SudokuBoard::SIZE; ++j) {
            int y = i * 2 + LABEL_OFFSET;
            int x = j * CELL_WIDTH + LABEL_OFFSET;
            char c = (i % 3 == 0 && j % 3 == 0) ? '#' : '+';
            mvwaddch(window_, y, x, c);
        }
    }

    // Draw cell contents
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            int y = row * 2 + LABEL_OFFSET + 1;
            int x = col * CELL_WIDTH + LABEL_OFFSET + 2; // Center in cell
            int value = board_.getCell(row, col);
            char ch = (value == 0) ? ' ' : ('0' + value);
            if (board_.isPreFilled(row, col)) {
                attron(COLOR_PAIR(1));
                mvwaddch(window_, y, x, ch);
                attroff(COLOR_PAIR(1));
            } else {
                mvwaddch(window_, y, x, ch);
            }
        }
    }

    wrefresh(window_);
}

void GameUI::showMenu() const noexcept {

}

void GameUI::handleInput() noexcept {

}