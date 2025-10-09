#include "GameController.hpp"
#include "SudokuBoard.hpp"
#include "GameUI.hpp"
#include <memory>

int main() {
    SudokuBoard board;

    auto ui = std::make_unique<GameUI>(board);

    GameController game(board, std::move(ui));

    game.run();
    
    return 0;
}