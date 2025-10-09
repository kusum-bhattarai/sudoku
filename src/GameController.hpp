#ifndef GAME_CONTROLLER_HPP
#define GAME_CONTROLLER_HPP

#include "SudokuBoard.hpp"
#include "GameUI.hpp"

class GameControllerTest_SubmitIncompleteBoard_Test;
class GameControllerTest_SubmitIncorrectBoard_Test;
class GameControllerTest_SubmitCorrectBoard_Test;
class GameControllerTest_UndoAction_Test;
class GameController {
    friend class GameControllerTest_SubmitIncompleteBoard_Test;
    friend class GameControllerTest_SubmitIncorrectBoard_Test;
    friend class GameControllerTest_SubmitCorrectBoard_Test;
    friend class GameControllerTest_UndoAction_Test;
public:
    GameController() noexcept;
    void run() noexcept;
    bool isRunning() const noexcept { return is_running_; }

private:
    void processInput(int ch) noexcept;     // Process input based on current focus
    void handleSubmit() noexcept;           // Handle submit action

    SudokuBoard board_;
    GameUI ui_;
    bool is_running_ = true;
};

#endif // GAME_CONTROLLER_HPP