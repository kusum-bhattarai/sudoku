#include "GameUI.hpp"

GameUI::GameUI(SudokuBoard& board) noexcept : board_(board), window_(nullptr) {
    window_ = initscr();
    if (!window_) {
        return;
    }
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_BLUE, COLOR_BLACK);
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_GREEN, COLOR_BLACK); 
    }
    keypad(window_, TRUE);
    noecho();
    cbreak();
    curs_set(0); 
    mousemask(0, NULL);
}

GameUI::~GameUI() noexcept {
    if (board_win_) {
        delwin(board_win_); 
    }
    if (menu_win_) {
        delwin(menu_win_);  
    }
    if (window_) {
        endwin();
    }
}

void GameUI::displayWelcomeScreen() const noexcept {
    if (!window_) return;

    werase(window_);
    wattron(window_, COLOR_PAIR(4));        // green color for welcome text

    int yMax, xMax;
    getmaxyx(window_, yMax, xMax);

    // welcome messages
    const char* line1 = "Wake up...";
    const char* line2 = "The Sudoku has you.";
    const char* line3 = "Press any key to enter the matrix.";

    // messages in the center of the screen
    mvwprintw(window_, yMax / 2 - 2, (xMax - strlen(line1)) / 2, "%s", line1);
    mvwprintw(window_, yMax / 2, (xMax - strlen(line2)) / 2, "%s", line2);
    mvwprintw(window_, yMax / 2 + 2, (xMax - strlen(line3)) / 2, "%s", line3);

    wattroff(window_, COLOR_PAIR(4));   
    wrefresh(window_);

    wgetch(window_); 
}


void GameUI::displayBoard() const noexcept {
    if (!window_) return;
    werase(window_);

    constexpr int CELL_WIDTH = 4;
    constexpr int GRID_WIDTH = SudokuBoard::SIZE * CELL_WIDTH + 1;
    constexpr int GRID_HEIGHT = SudokuBoard::SIZE * 2 + 1;
    constexpr int LABEL_OFFSET = 2; 

    int yMax, xMax;
    getmaxyx(window_, yMax, xMax);

    int start_y = (yMax - GRID_HEIGHT - LABEL_OFFSET) / 2;
    int start_x = (xMax - GRID_WIDTH - LABEL_OFFSET) / 2;

    // Draw column labels (A-I)
    for (int col = 0; col < SudokuBoard::SIZE; ++col) {
        mvwaddch(window_, start_y, start_x + col * CELL_WIDTH + LABEL_OFFSET + (CELL_WIDTH/2), 'A' + col);
    }

    // Draw row labels (1-9)
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        mvwaddch(window_, start_y + row * 2 + 1 + LABEL_OFFSET, start_x, '1' + row);
    }

    // Draw grid lines (remains the same)
    for (int i = 0; i <= SudokuBoard::SIZE; ++i) {
        int x = start_x + i * CELL_WIDTH + LABEL_OFFSET;
        char vchar = (i % 3 == 0) ? '#' : '|';
        mvwvline(window_, start_y + LABEL_OFFSET, x, vchar, GRID_HEIGHT -1);
    }
    for (int i = 0; i <= SudokuBoard::SIZE; ++i) {
        int y = start_y + i * 2 + LABEL_OFFSET;
        char hchar = (i % 3 == 0) ? '=' : '-';
        mvwhline(window_, y, start_x + LABEL_OFFSET, hchar, GRID_WIDTH-1);
    }
    for (int i = 0; i <= SudokuBoard::SIZE; ++i) {
        for (int j = 0; j <= SudokuBoard::SIZE; ++j) {
            int y = start_y + i * 2 + LABEL_OFFSET;
            int x = start_x + j * CELL_WIDTH + LABEL_OFFSET;
            char c = (i % 3 == 0 && j % 3 == 0) ? '#' : ( (i%3==0) ? '=': ((j%3==0)?'#':'|'));
            mvwaddch(window_, y, x, c);
        }
    }

    // Draw cell contents
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            int y = start_y + row * 2 + 1 + LABEL_OFFSET;
            int x = start_x + col * CELL_WIDTH + 2 + LABEL_OFFSET;
            int value = board_.getCell(row, col);
            char ch = (value == 0) ? '.' : ('0' + value);

            int attribute = A_NORMAL; 
            if (board_.isPreFilled(row, col)) {
                attribute = COLOR_PAIR(1); // Blue for pre-filled
            } else if (value != 0) {
                attribute = COLOR_PAIR(2); // Yellow for user-entered
            }
            if (row == cursor_row_ && col == cursor_col_) {
                attribute |= A_REVERSE;
            }
            wattron(window_, attribute);
            mvwaddch(window_, y, x, ch);
            wattroff(window_, attribute);
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