#ifndef GAME_UI_HPP
#define GAME_UI_HPP

#include <ncurses.h>
#include "SudokuBoard.hpp"
#include "IGameUI.hpp" 

class GameUI : public IGameUI { // inheriting from IGameUI
public:
    explicit GameUI(SudokuBoard& board) noexcept;
    ~GameUI() noexcept override;
    GameUI(const GameUI&) = delete;
    GameUI& operator=(const GameUI&) = delete;

    // --- IGameUI Interface Implementations ---
    void displayBoard() const noexcept override;
    int getPressedKey() const noexcept override;
    void displayMessage(const std::string& message) const noexcept override;
    void flashScreen() const noexcept override;

    // --- State Setters (called by GameController) ---
    void setFocus(FocusState new_focus) noexcept override;
    void setCursorPosition(int row, int col) noexcept override;
    void setSelectedMenuItem(int item) noexcept override;
    void setErrors(const std::vector<std::pair<int, int>>& errors) noexcept override;

    // --- State Getters (needed by GameController) ---
    std::pair<int, int> getCursorPosition() const noexcept override;
    FocusState getFocus() const noexcept override;
    const std::vector<std::string>& getMenuItems() const noexcept override;
    int getSelectedMenuItem() const noexcept override;
    
    void displayWelcomeScreen() const noexcept;
    const std::vector<std::vector<int>>& getBoard() const noexcept { return board_.getBoard(); }
    std::string getLastMessage() const { return last_message_; }


protected:
    void drawBoardWindow() const noexcept; 
    void drawMenuWindow() const noexcept;

    SudokuBoard& board_;                        // Reference to board
    WINDOW* window_;                            // ncurses window
    int cursor_row_ = 0;                        // Cursor position row
    int cursor_col_ = 0;                        // Cursor position col
    FocusState focus_ = FocusState::BOARD;      // Start with focus on the board
    int selected_menu_item_ = 0;                // Selected menu item index
    const std::vector<std::string> menu_items_ = {"Submit", "Undo", "Hint", "New Game", "Quit"};
    mutable std::string last_message_;         // Store last message for testing
    std::vector<std::pair<int, int>> error_cells_;  // Cells that are in error state

    WINDOW* board_win_ = nullptr; 
    WINDOW* menu_win_ = nullptr;
};

#endif // GAME_UI_HPP