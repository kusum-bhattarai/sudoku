#include <gtest/gtest.h>
#include <ncurses.h>
#include "GameController.hpp"
#include "SudokuBoard.hpp"
#include "IGameUI.hpp"
#include <memory>
#include <vector>

// This is our mock UI class for testing the GameController logic.
class MockGameUI : public IGameUI {
public:
    // --- Mock Controls ---
    void setNextKey(int key) {
        key_to_return_ = key;
    }

    // --- Mock State Variables (for assertions) ---
    std::string last_displayed_message;
    int flash_screen_call_count = 0;
    std::vector<std::pair<int, int>> last_set_errors;

private:
    FocusState focus_ = FocusState::BOARD;
    std::pair<int, int> cursor_pos_ = {0, 0};
    int selected_menu_item_ = 0;
    int key_to_return_ = 0;
    const std::vector<std::string> menu_items_ = {"Submit", "Undo", "Hint", "New Game", "Quit"};

public:
    // --- IGameUI Interface Implementation ---
    void displayBoard() const noexcept override {}

    int getPressedKey() const noexcept override {
        return key_to_return_;
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

// New test fixture for GameController
class GameControllerTest : public ::testing::Test {
protected:
    SudokuBoard board;
    MockGameUI* mock_ui_ptr; // Raw pointer for setting up and inspecting the mock
    std::unique_ptr<GameController> controller;

    // This function runs before each test
    void SetUp() override {
        auto owned_ui = std::make_unique<MockGameUI>();
        mock_ui_ptr = owned_ui.get(); // Get the raw pointer before moving ownership
        controller = std::make_unique<GameController>(board, std::move(owned_ui));
    }

    // Helper to simulate a sequence of inputs to the controller
    void simulateKeyPresses(const std::vector<int>& keys) {
        for (int key : keys) {
            mock_ui_ptr->setNextKey(key);
            controller->processInput(mock_ui_ptr->getPressedKey());
        }
    }
};

TEST_F(GameControllerTest, SubmitIncompleteBoard) {
    // Arrange: Board is empty by default
    
    // Act: Simulate tabbing to menu and pressing enter on "Submit"
    simulateKeyPresses({'\t', '\n'});

    // Assert
    EXPECT_EQ(mock_ui_ptr->last_displayed_message, "Board is not full yet!");
    EXPECT_TRUE(controller->isRunning());
}

TEST_F(GameControllerTest, SubmitIncorrectBoard) {
    // Arrange: Fill the board with an invalid solution
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            board.setCell(i, j, 1);
        }
    }

    // Act: Simulate submitting
    simulateKeyPresses({'\t', '\n'});

    // Assert
    EXPECT_EQ(mock_ui_ptr->last_displayed_message, "Your mistakes are highlighted in red!");
    EXPECT_FALSE(mock_ui_ptr->last_set_errors.empty()); // Check that errors were actually set
    EXPECT_TRUE(controller->isRunning());
}

TEST_F(GameControllerTest, SubmitCorrectBoard) {
    // Arrange: Fill the board with a valid, complete solution
    std::random_device rd;
    std::mt19937 g(rd());
    board.solveBoard(g);

    // Act: Simulate submitting
    simulateKeyPresses({'\t', '\n'});

    // Assert
    EXPECT_EQ(mock_ui_ptr->last_displayed_message, "Congratulations! You solved it!");
    EXPECT_FALSE(controller->isRunning());
}

TEST_F(GameControllerTest, UndoAction_Success) {
    // Arrange: Make a move on the board
    simulateKeyPresses({'5'});
    ASSERT_EQ(board.getCell(0, 0), 5);

    // Act: Navigate to "Undo" and select it
    simulateKeyPresses({'\t', KEY_DOWN, '\n'});

    // Assert
    EXPECT_EQ(board.getCell(0, 0), 0);
    EXPECT_EQ(mock_ui_ptr->last_displayed_message, "Last move undone!");
}

TEST_F(GameControllerTest, UndoAction_NothingToUndo) {
    // Arrange: Board is in its initial state

    // Act: Navigate to "Undo" and select it
    simulateKeyPresses({'\t', KEY_DOWN, '\n'});

    // Assert
    EXPECT_EQ(mock_ui_ptr->last_displayed_message, "Nothing to undo!");
}

TEST_F(GameControllerTest, HintAction_Success) {
    // Arrange: Board is empty, cursor is at (0, 0)
    ASSERT_EQ(board.getCell(0, 0), 0);

    // Act: Navigate to "Hint" and select it
    simulateKeyPresses({'\t', KEY_DOWN, KEY_DOWN, '\n'});

    // Assert
    EXPECT_NE(board.getCell(0, 0), 0); // Cell should now have a hint
    EXPECT_EQ(mock_ui_ptr->last_displayed_message, "Hint provided!");
}

TEST_F(GameControllerTest, HintAction_OnPreFilledCellFails) {
    // Arrange: Cell (0,0) is pre-filled
    board.setCell(0, 0, 5);
    board.setPreFilled(0, 0, true);
    ASSERT_EQ(mock_ui_ptr->flash_screen_call_count, 0);

    // Act: Navigate to "Hint" and select it
    simulateKeyPresses({'\t', KEY_DOWN, KEY_DOWN, '\n'});

    // Assert
    EXPECT_EQ(board.getCell(0, 0), 5); // Value should not have changed
    EXPECT_EQ(mock_ui_ptr->flash_screen_call_count, 1);
}

TEST_F(GameControllerTest, QuitActionFromMenu) {
    // Arrange: Game is running
    ASSERT_TRUE(controller->isRunning());

    // Act: Navigate to "Quit" and select it
    simulateKeyPresses({'\t', KEY_UP, '\n'}); // KEY_UP wraps around to Quit

    // Assert
    EXPECT_FALSE(controller->isRunning());
}

TEST_F(GameControllerTest, InputOnBoard_CannotChangePreFilledCell) {
    // Arrange: Cell (0,0) is pre-filled
    board.setCell(0, 0, 5);
    board.setPreFilled(0, 0, true);
    ASSERT_EQ(mock_ui_ptr->flash_screen_call_count, 0);

    // Act: Try to type '7' into the cell
    simulateKeyPresses({'7'});

    // Assert
    EXPECT_EQ(board.getCell(0, 0), 5); // Value should not change
    EXPECT_EQ(mock_ui_ptr->flash_screen_call_count, 1);
}