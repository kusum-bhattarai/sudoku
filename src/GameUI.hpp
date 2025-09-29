#ifndef GAME_UI_HPP
#define GAME_UI_HPP

#include <ncurses.h>
#include "SudokuBoard.hpp"

class GameUI {
public:
    explicit GameUI(SudokuBoard& board) noexcept;   // Constructor takes board reference
    ~GameUI() noexcept;                             // Cleanup ncurses
    GameUI(const GameUI&) = delete;                 // No copy
    GameUI& operator=(const GameUI&) = delete;

    void displayWelcomeScreen() const noexcept;
    void displayBoard() const noexcept;          
    void showMenu() const noexcept;             
    bool handleInput() noexcept;               

    // Getter for testing
    const std::vector<std::vector<int>>& getBoard() const noexcept { return board_.getBoard(); }
    std::pair<int, int> getCursorPosition() const noexcept { return {cursor_row_, cursor_col_}; }

protected:
    SudokuBoard& board_;                        // Reference to board
    WINDOW* window_;                            // ncurses window
    int cursor_row_ = 0;                        // Cursor position row
    int cursor_col_ = 0;                        // Cursor position col

    // Separate sub-windows for board and menu
    WINDOW* board_win_ = nullptr; 
    WINDOW* menu_win_ = nullptr;
};

#endif // GAME_UI_HPP