#include "GameController.hpp"
#include "GameUI.hpp" 

// The constructor now initializes the board reference and takes ownership of the UI pointer
GameController::GameController(SudokuBoard& board, std::unique_ptr<IGameUI> ui) noexcept
    : board_(board), ui_(std::move(ui)) {}

void GameController::run() noexcept {
    if (auto* concrete_ui = dynamic_cast<GameUI*>(ui_.get())) {
        concrete_ui->displayWelcomeScreen();
    }
    
    board_.generatePuzzle(SudokuBoard::Difficulty::Easy);

    while (is_running_) {
        ui_->displayBoard();
        int ch = ui_->getPressedKey();
        processInput(ch);
    }
}

void GameController::handleSubmit() noexcept {
    if (!board_.isFull()) {
        ui_->displayMessage("Board is not full yet!");
        return;
    }

    if (board_.isValid()) {
        ui_->displayMessage("Congratulations! You solved it!");
        is_running_ = false;
    } else {
        auto errors = board_.findErrors();
        ui_->setErrors(errors);
        ui_->displayMessage("Your mistakes are highlighted in red!");
    }
}

void GameController::handleUndo() noexcept {
    if (board_.undo()) {
        ui_->displayMessage("Last move undone!");
    } else {
        ui_->displayMessage("Nothing to undo!");
    }
}

void GameController::handleHint() noexcept {
    auto [row, col] = ui_->getCursorPosition();

    // Check if the cell is part of the original puzzle
    if (board_.isPreFilled(row, col)) {
        ui_->displayMessage("Cannot provide hint for a pre-filled cell.");
        return;
    }

    // Check if the user has run out of hints
    if (board_.getHintsUsed() >= SudokuBoard::MAX_HINTS) {
        ui_->displayMessage("You have used all " + std::to_string(SudokuBoard::MAX_HINTS) + " hints.");
        return;
    }
    
    // If checks pass, try to find a valid hint
    std::random_device rd;
    std::mt19937 g(rd());
    auto hint = board_.getHint(row, col, g);

    if (hint.has_value()) {
        board_.setCell(row, col, hint.value());
        ui_->displayMessage("Hint provided! (" + std::to_string(board_.getHintsUsed()) + "/" + std::to_string(SudokuBoard::MAX_HINTS) + " used)");
    } else {
        // This means the board is in an unsolvable state from its current configuration
        ui_->displayMessage("No hint available. Check for mistakes on the board.");
    }
}

void GameController::handleNewGame() noexcept {
    board_.generatePuzzle(SudokuBoard::Difficulty::Easy);
    ui_->setFocus(FocusState::BOARD);
    ui_->displayMessage("New Easy game started!");
}

void GameController::processInput(int ch) noexcept {        
    if (ui_->getFocus() == FocusState::BOARD) {
        ui_->setErrors({});
        auto [row, col] = ui_->getCursorPosition();
        switch (ch) {
            case 'q':
            case 'Q':
                is_running_ = false;
                break;
            case '\t':
                ui_->setFocus(FocusState::MENU);
                break;
            case KEY_RIGHT: {
                int index = row * 9 + col;
                index = (index + 1) % 81;
                ui_->setCursorPosition(index / 9, index % 9);
                break;
            }
            case KEY_LEFT: {
                int index = row * 9 + col;
                index = (index - 1 + 81) % 81;
                ui_->setCursorPosition(index / 9, index % 9);
                break;
            }
            case KEY_UP: {
                int index = col * 9 + row;
                index = (index - 1 + 81) % 81;
                ui_->setCursorPosition(index % 9, index / 9);
                break;
            }
            case KEY_DOWN: {
                int index = col * 9 + row;
                index = (index + 1) % 81;
                ui_->setCursorPosition(index % 9, index / 9);
                break;
            }
            case '1' ... '9':
                if (!board_.isPreFilled(row, col)) {
                    board_.setCell(row, col, ch - '0');
                } else {
                    ui_->flashScreen();
                }
                break;
            case KEY_BACKSPACE:
            case 127:
            case '0':
                if (!board_.isPreFilled(row, col)) {
                    board_.setCell(row, col, 0);
                }
                break;
        }
    } else { // Focus is on MENU
        int current_item = ui_->getSelectedMenuItem();
        const auto& menu_items = ui_->getMenuItems();
        switch (ch) {
            case '\t':
                ui_->setFocus(FocusState::BOARD);
                break;
            case KEY_UP:
                ui_->setSelectedMenuItem((current_item - 1 + menu_items.size()) % menu_items.size());
                break;
            case KEY_DOWN:
                ui_->setSelectedMenuItem((current_item + 1) % menu_items.size());
                break;
            case '\n':
            case KEY_ENTER: { 
                const std::string& selected_item = menu_items[current_item];
                if (selected_item == "Quit") {
                    is_running_ = false;
                } else if (selected_item == "Submit") {
                    handleSubmit(); 
                } else if (selected_item == "Undo") {
                    handleUndo();
                } else if (selected_item == "Hint") {
                    handleHint();
                } else if (selected_item == "New Game") { 
                    handleNewGame();                     
                }
                break;
            }
        }
    }
}