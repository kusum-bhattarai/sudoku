#ifndef GAME_CONTROLLER_HPP
#define GAME_CONTROLLER_HPP

#include "SudokuBoard.hpp"
#include "IGameUI.hpp"
#include <memory> // Required for std::unique_ptr

class GameController {
public:
    explicit GameController(SudokuBoard& board, std::unique_ptr<IGameUI> ui) noexcept;
    
    void run() noexcept;
    bool isRunning() const noexcept { return is_running_; }

private:
    void processInput(int ch) noexcept;     // Process input based on current focus
    void handleSubmit() noexcept;           // Handle submit action
    void handleHint() noexcept;             // Handle hint action
    void handleUndo() noexcept;             // Handle undo action
    
    SudokuBoard& board_;
    std::unique_ptr<IGameUI> ui_; // Owns a UI that implements the interface
    bool is_running_ = true;
};

#endif // GAME_CONTROLLER_HPP