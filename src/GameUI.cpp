#include "GameUI.hpp"

GameUI::GameUI(SudokuBoard& board) noexcept : board_(board), window_(nullptr) {
    window_ = initscr();
    if (!window_) {
        return;
    }
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_BLUE, COLOR_BLACK);         // for pre-filled cells
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);       // for user-filled cells
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

            // Highlight the current cursor position
            if (row == cursor_row_ && col == cursor_col_) {
                wattron(window_, A_REVERSE);
            }

            if (board_.isPreFilled(row, col)) {
                wattron(window_, COLOR_PAIR(1));
                mvwaddch(window_, y, x, ch);
                wattroff(window_, COLOR_PAIR(1));
            } else {
                 if(value != 0){
                    wattron(window_, COLOR_PAIR(2));
                    mvwaddch(window_, y, x, ch);
                    wattroff(window_, COLOR_PAIR(2));
                } else {
                    mvwaddch(window_, y, x, ch);
                }
            }

            // Turn off highlight
            if (row == cursor_row_ && col == cursor_col_) {
                wattroff(window_, A_REVERSE);
            }
        }
    }

    wrefresh(window_);
}


void GameUI::showMenu() const noexcept {

}

bool GameUI::handleInput() noexcept {
    if (!window_) return false;
    int ch = wgetch(window_);
    switch (ch) {
        case 'q':
        case 'Q':
            return false; // Signal to quit

        case KEY_RIGHT: {
            int index = cursor_row_ * SudokuBoard::SIZE + cursor_col_;
            index = (index + 1) % (SudokuBoard::SIZE * SudokuBoard::SIZE);
            cursor_row_ = index / SudokuBoard::SIZE;
            cursor_col_ = index % SudokuBoard::SIZE;
            break;
        }
        case KEY_LEFT: {
            int index = cursor_row_ * SudokuBoard::SIZE + cursor_col_;
            index = (index - 1 + (SudokuBoard::SIZE * SudokuBoard::SIZE)) % (SudokuBoard::SIZE * SudokuBoard::SIZE);
            cursor_row_ = index / SudokuBoard::SIZE;
            cursor_col_ = index % SudokuBoard::SIZE;
            break;
        }
        case KEY_DOWN: {
            int index = cursor_col_ * SudokuBoard::SIZE + cursor_row_;
            index = (index + 1) % (SudokuBoard::SIZE * SudokuBoard::SIZE);
            cursor_col_ = index / SudokuBoard::SIZE;
            cursor_row_ = index % SudokuBoard::SIZE;
            break;
        }
        case KEY_UP: {
            int index = cursor_col_ * SudokuBoard::SIZE + cursor_row_;
            index = (index - 1 + (SudokuBoard::SIZE * SudokuBoard::SIZE)) % (SudokuBoard::SIZE * SudokuBoard::SIZE);
            cursor_col_ = index / SudokuBoard::SIZE;
            cursor_row_ = index % SudokuBoard::SIZE;
            break;
        }

        case '1' ... '9':
            if (!board_.isPreFilled(cursor_row_, cursor_col_)) {
                int value = ch - '0';
                board_.setCell(cursor_row_, cursor_col_, value);
            } else {
                flash();
            }
            break;

        case KEY_BACKSPACE:
        case 127: 
        case '0':
             if (!board_.isPreFilled(cursor_row_, cursor_col_)) {
                board_.setCell(cursor_row_, cursor_col_, 0);
            }
            break;

        default:
            break;
    }

    displayBoard();
    return true;
}