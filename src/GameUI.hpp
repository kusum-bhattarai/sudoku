#ifndef GAME_UI_HPP
#define GAME_UI_HPP

#include <ncurses.h>
#include "SudokuBoard.hpp"

class GameUI {
public:
    explicit GameUI(SudokuBoard& board) noexcept; // Constructor takes board reference
    ~GameUI() noexcept;                          // Cleanup ncurses
    GameUI(const GameUI&) = delete;             // No copy
    GameUI& operator=(const GameUI&) = delete;

    void displayBoard() const noexcept;          // Placeholder for board display
    void showMenu() const noexcept;             // Placeholder for menu
    bool handleInput() noexcept;                // changed the type to bool to return false on quit

    // Getter for testing
    const std::vector<std::vector<int>>& getBoard() const noexcept { return board_.getBoard(); }
    std::pair<int, int> getCursorPosition() const noexcept { return {cursor_row_, cursor_col_}; }

private:
    SudokuBoard& board_;                        // Reference to board
    WINDOW* window_;                            // ncurses window
    int cursor_row_ = 0;                        // Cursor position row
    int cursor_col_ = 0;                        // Cursor position col
};

#endif // GAME_UI_HPP