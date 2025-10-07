#include "GameController.hpp"

GameController::GameController() noexcept : ui_(board_) {
    // constructor to initialize the UI with the board
}

void GameController::run() noexcept {
    board_.generatePuzzle(SudokuBoard::Difficulty::Easy);
    
    ui_.displayWelcomeScreen();

    while (ui_.handleInput()) {
        // The loop continues
    }
}