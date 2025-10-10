#include <gtest/gtest.h>
#include <ncurses.h>
#include "GameController.hpp"
#include "SudokuBoard.hpp"
#include "IGameUI.hpp"
#include <memory>
#include <vector>

class MockGameUI : public IGameUI {
public:
    // --- Mock Controls ---
    void setKeySequence(const std::vector<int>& keys) {
        keys_to_return_ = keys;
        key_index_ = 0;
    }

    // --- Mock State Variables (for assertions) ---
    std::string last_displayed_message;
    int flash_screen_call_count = 0;
    std::vector<std::pair<int, int>> last_set_errors;

    // --- Add the displayDifficultyMenu mock implementation ---
    void displayDifficultyMenu(int selected_difficulty) const noexcept override {
        // We don't need to do anything here for the logic tests
    }

private:
    FocusState focus_ = FocusState::BOARD;
    std::pair<int, int> cursor_pos_ = {0, 0};
    int selected_menu_item_ = 0;
    std::vector<int> keys_to_return_;
    mutable size_t key_index_ = 0;
    const std::vector<std::string> menu_items_ = {"Submit", "Undo", "Hint", "New Game", "Quit"};

public:
    // --- IGameUI Interface Implementation ---
    void displayBoard() const noexcept override {}

    int getPressedKey() const noexcept override {
        if (key_index_ < keys_to_return_.size()) {
            return keys_to_return_[key_index_++];
        }
        return 0; // Return 0 if we run out of keys
    }

    void displayMessage(const std::string& message) const noexcept override {
        const_cast<MockGameUI*>(this)->last_displayed_message = message;
    }

    void flashScreen() const noexcept override {
        const_cast<MockGameUI*>(this)->flash_screen_call_count++;
    }

    void setFocus(FocusState new_focus) noexcept override {
        focus_ = new_focus;
    }

    void setCursorPosition(int row, int col) noexcept override {
        cursor_pos_ = {row, col};
    }

    void setSelectedMenuItem(int item) noexcept override {
        selected_menu_item_ = item;
    }

    void setErrors(const std::vector<std::pair<int, int>>& errors) noexcept override {
        last_set_errors = errors;
    }

    std::pair<int, int> getCursorPosition() const noexcept override {
        return cursor_pos_;
    }

    FocusState getFocus() const noexcept override {
        return focus_;
    }

    const std::vector<std::string>& getMenuItems() const noexcept override {
        return menu_items_;
    }

    int getSelectedMenuItem() const noexcept override {
        return selected_menu_item_;
    }
};

// Test fixture for GameController
class GameControllerTest : public ::testing::Test {
protected:
    SudokuBoard board;
    MockGameUI* mock_ui_ptr; // Raw pointer for setting up and inspecting the mock
    std::unique_ptr<GameController> controller;

    void SetUp() override {
        auto owned_ui = std::make_unique<MockGameUI>();
        mock_ui_ptr = owned_ui.get();
        controller = std::make_unique<GameController>(board, std::move(owned_ui));
    }

    // CORRECTED: This helper now correctly simulates a sequence of inputs
    void simulateKeyPresses(const std::vector<int>& keys) {
        mock_ui_ptr->setKeySequence(keys);
        for (size_t i = 0; i < keys.size(); ++i) {
            controller->processInput(mock_ui_ptr->getPressedKey());
        }
    }
};

TEST_F(GameControllerTest, SubmitIncompleteBoard) {
    simulateKeyPresses({'\t', '\n'});
    EXPECT_EQ(mock_ui_ptr->last_displayed_message, "Board is not full yet!");
    EXPECT_TRUE(controller->isRunning());
}

TEST_F(GameControllerTest, SubmitIncorrectBoard) {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            board.setCell(i, j, 1);
        }
    }
    simulateKeyPresses({'\t', '\n'});
    EXPECT_EQ(mock_ui_ptr->last_displayed_message, "Your mistakes are highlighted in red!");
    EXPECT_FALSE(mock_ui_ptr->last_set_errors.empty());
    EXPECT_TRUE(controller->isRunning());
}

TEST_F(GameControllerTest, SubmitCorrectBoard) {
    std::random_device rd;
    std::mt19937 g(rd());
    board.solveBoard(g);
    simulateKeyPresses({'\t', '\n'});
    EXPECT_EQ(mock_ui_ptr->last_displayed_message, "Congratulations! You solved it!");
    EXPECT_FALSE(controller->isRunning());
}

TEST_F(GameControllerTest, UndoAction_Success) {
    simulateKeyPresses({'5'});
    ASSERT_EQ(board.getCell(0, 0), 5);
    simulateKeyPresses({'\t', KEY_DOWN, '\n'});
    EXPECT_EQ(board.getCell(0, 0), 0);
    EXPECT_EQ(mock_ui_ptr->last_displayed_message, "Last move undone!");
}

TEST_F(GameControllerTest, UndoAction_NothingToUndo) {
    simulateKeyPresses({'\t', KEY_DOWN, '\n'});
    EXPECT_EQ(mock_ui_ptr->last_displayed_message, "Nothing to undo!");
}

TEST_F(GameControllerTest, HintAction_Success) {
    ASSERT_EQ(board.getCell(0, 0), 0);
    simulateKeyPresses({'\t', KEY_DOWN, KEY_DOWN, '\n'});
    EXPECT_NE(board.getCell(0, 0), 0);
    EXPECT_EQ(mock_ui_ptr->last_displayed_message, "Hint provided! (1/3 used)");
}

// CORRECTED: This test now checks for the correct message instead of a screen flash
TEST_F(GameControllerTest, HintAction_OnPreFilledCellFails) {
    board.setCell(0, 0, 5);
    board.setPreFilled(0, 0, true);
    simulateKeyPresses({'\t', KEY_DOWN, KEY_DOWN, '\n'});
    EXPECT_EQ(board.getCell(0, 0), 5);
    EXPECT_EQ(mock_ui_ptr->last_displayed_message, "Cannot provide hint for a pre-filled cell.");
}

TEST_F(GameControllerTest, QuitActionFromMenu) {
    ASSERT_TRUE(controller->isRunning());
    simulateKeyPresses({'\t', KEY_UP, '\n'});
    EXPECT_FALSE(controller->isRunning());
}

TEST_F(GameControllerTest, InputOnBoard_CannotChangePreFilledCell) {
    board.setCell(0, 0, 5);
    board.setPreFilled(0, 0, true);
    simulateKeyPresses({'7'});
    EXPECT_EQ(board.getCell(0, 0), 5);
    EXPECT_EQ(mock_ui_ptr->flash_screen_call_count, 1);
}

TEST_F(GameControllerTest, SelectDifficulty_NavigatesAndSelectsHardMode) {
    std::vector<int> key_presses = {KEY_DOWN, KEY_DOWN, '\n'};
    mock_ui_ptr->setKeySequence(key_presses);
    SudokuBoard::Difficulty selected_difficulty = controller->selectDifficulty();
    EXPECT_EQ(selected_difficulty, SudokuBoard::Difficulty::Hard);
}