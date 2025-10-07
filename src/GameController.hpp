#ifndef GAME_CONTROLLER_HPP
#define GAME_CONTROLLER_HPP

#include "SudokuBoard.hpp"
#include "GameUI.hpp"

class GameController {
public:
    GameController() noexcept;
    void run() noexcept;

private:
    void processInput(int ch) noexcept;     // Process input based on current focus

    SudokuBoard board_;
    GameUI ui_;
    bool is_running_ = true;
};

#endif // GAME_CONTROLLER_HPP