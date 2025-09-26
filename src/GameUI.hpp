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
    void handleInput() noexcept;                // Placeholder for input handling

    // Getter for testing
    const std::vector<std::vector<int>>& getBoard() const noexcept { return board_.getBoard(); }

private:
    SudokuBoard& board_;                        // Reference to board
    WINDOW* window_;                            // ncurses window
};

#endif // GAME_UI_HPP