#include "GameUI.hpp"
#include <string>

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
        init_pair(5, COLOR_BLACK, COLOR_WHITE);
    }
    keypad(window_, TRUE);
    noecho();
    cbreak();
    curs_set(0); 
    mousemask(0, NULL);

    // --- Creating sub-windows for board and menu ---
    int yMax, xMax;
    getmaxyx(window_, yMax, xMax);

    // Windows dimensions
    constexpr int BOARD_WIN_HEIGHT = 19;         // 9 * 2 + 1
    constexpr int BOARD_WIN_WIDTH = 37;          // 9 * 4 + 1
    constexpr int MENU_WIN_WIDTH = 25;
    constexpr int SPACING = 2;

    // Center the whole layout
    int total_layout_width = BOARD_WIN_WIDTH + SPACING + MENU_WIN_WIDTH;
    int start_y = (yMax - BOARD_WIN_HEIGHT) / 2;
    int start_x = (xMax - total_layout_width) / 2;

    board_win_ = newwin(BOARD_WIN_HEIGHT, BOARD_WIN_WIDTH, start_y, start_x);
    menu_win_ = newwin(BOARD_WIN_HEIGHT, MENU_WIN_WIDTH, start_y, start_x + BOARD_WIN_WIDTH + SPACING);
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

void GameUI::displayBoard() const noexcept {
    if (!window_ || !board_win_ || !menu_win_) return;
    werase(window_); // Clear the main screen
    wrefresh(window_);

    drawBoardWindow(); // Draw the board in its window
    drawMenuWindow();  // Draw the menu in its window
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

void GameUI::drawBoardWindow() const noexcept {
    werase(board_win_);

    // Draw grid lines
    for (int i = 0; i <= 9; ++i) {
        if (i % 3 == 0) wattron(board_win_, A_BOLD);
        mvwvline(board_win_, 0, i * 4, ACS_VLINE, 19);
        mvwhline(board_win_, i * 2, 0, ACS_HLINE, 37);
        if (i % 3 == 0) wattroff(board_win_, A_BOLD);
    }

    // Draw cell contents
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            int value = board_.getCell(row, col);
            char ch = (value == 0) ? '.' : ('0' + value);

            int attribute = A_NORMAL;
            if (board_.isPreFilled(row, col)) {
                attribute = COLOR_PAIR(1);
            } else if (value != 0) {
                attribute = COLOR_PAIR(2);
            }
            if (focus_ == FocusState::BOARD && row == cursor_row_ && col == cursor_col_) {
                attribute |= A_REVERSE;
            }

            wattron(board_win_, attribute);
            mvwaddch(board_win_, row * 2 + 1, col * 4 + 2, ch);
            wattroff(board_win_, attribute);
        }
    }
    box(board_win_, 0, 0);
    wrefresh(board_win_);
}

void GameUI::drawMenuWindow() const noexcept {
    werase(menu_win_);
    box(menu_win_, 0, 0);

    // Title
    wattron(menu_win_, A_BOLD | COLOR_PAIR(4));
    mvwprintw(menu_win_, 1, (25 - 10) / 2, "[ MATRIX ]");
    wattroff(menu_win_, A_BOLD | COLOR_PAIR(4));

    // Controls
    mvwprintw(menu_win_, 3, 2, "[ CONTROLS ]");
    mvwprintw(menu_win_, 4, 3, "Arrows: Move/Select");
    mvwprintw(menu_win_, 5, 3, "Tab   : Switch Focus");
    mvwprintw(menu_win_, 6, 3, "Enter : Select Action");

    mvwprintw(menu_win_, 9, 2, "[ ACTIONS ]");
    for (int i = 0; i < menu_items_.size(); ++i) {
        if (focus_ == FocusState::MENU && i == selected_menu_item_) {
            wattron(menu_win_, COLOR_PAIR(5)); // Highlight selected item
        }
        mvwprintw(menu_win_, 10 + i, 3, menu_items_[i].c_str());
        if (focus_ == FocusState::MENU && i == selected_menu_item_) {
            wattroff(menu_win_, COLOR_PAIR(5));
        }
    }

    wrefresh(menu_win_);
}

void GameUI::displayMessage(const std::string& message) const noexcept {
    int yMax, xMax;
    getmaxyx(window_, yMax, xMax);

    int msg_width = message.length() + 4;
    int msg_height = 3;
    int start_y = (yMax - msg_height) / 2;
    int start_x = (xMax - msg_width) / 2;

    WINDOW* msg_win = newwin(msg_height, msg_width, start_y, start_x);
    box(msg_win, 0, 0);
    mvwprintw(msg_win, 1, 2, message.c_str());
    wrefresh(msg_win);

    wgetch(window_); // Wait for any key press

    delwin(msg_win);
}

int GameUI::getPressedKey() const noexcept {
    return wgetch(window_);
}

void GameUI::setFocus(FocusState new_focus) noexcept {
    focus_ = new_focus;
}
void GameUI::setCursorPosition(int row, int col) noexcept {
    cursor_row_ = row;
    cursor_col_ = col;
}
void GameUI::setSelectedMenuItem(int item) noexcept {
    selected_menu_item_ = item;
}
void GameUI::flashScreen() const noexcept {
    flash();
}