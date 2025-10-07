#ifndef GAME_CONTROLLER_HPP
#define GAME_CONTROLLER_HPP

#include "SudokuBoard.hpp"
#include "GameUI.hpp"

class GameController {
public:
    GameController() noexcept;
    void run() noexcept;

private:
    SudokuBoard board_;
    GameUI ui_;
};

#endif // GAME_CONTROLLER_HPP