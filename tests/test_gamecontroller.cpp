#include <gtest/gtest.h>
#include "GameController.hpp"
#include "SudokuBoard.hpp"

// We make the test fixture a friend of the GameController to access its private members
class GameControllerTest : public ::testing::Test {
protected:
    friend class GameController;
    GameController controller;
};

TEST_F(GameControllerTest, SubmitIncompleteBoard) {
    ASSERT_FALSE(controller.board_.isFull());
    
    controller.ui_.setFocus(GameUI::FocusState::MENU);
    controller.ui_.setSelectedMenuItem(0); // 0 is "Submit"
    controller.processInput('\n');

    EXPECT_TRUE(controller.isRunning());
    // Verify that the correct message was passed to the UI
    EXPECT_EQ(controller.ui_.getLastMessage(), "Board is not full yet!");
}

TEST_F(GameControllerTest, SubmitIncorrectBoard) {
    // Fill the board, but make it incorrect
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            controller.board_.setCell(i, j, 1); // Fill every cell with '1'
        }
    }
    ASSERT_TRUE(controller.board_.isFull());
    ASSERT_FALSE(controller.board_.isValid());

    // Simulate selecting "Submit" and pressing Enter
    controller.ui_.setFocus(GameUI::FocusState::MENU);
    controller.ui_.setSelectedMenuItem(0);
    controller.processInput('\n');

    // The game should still be running
    EXPECT_TRUE(controller.isRunning());
}

TEST_F(GameControllerTest, SubmitCorrectBoard) {
    // Generate a full, solved board
    std::random_device rd;
    std::mt19937 g(rd());
    controller.board_.solveBoard(g);
    ASSERT_TRUE(controller.board_.isFull());
    ASSERT_TRUE(controller.board_.isValid());

    // Simulate selecting "Submit" and pressing Enter
    controller.ui_.setFocus(GameUI::FocusState::MENU);
    controller.ui_.setSelectedMenuItem(0);
    controller.processInput('\n');

    // The game should stop running
    EXPECT_FALSE(controller.isRunning());
}

TEST_F(GameControllerTest, UndoAction) {
    // Make a move on the board
    controller.board_.setCell(0, 0, 5);
    ASSERT_EQ(controller.board_.getCell(0, 0), 5);

    // Simulate selecting "Undo" and pressing Enter
    controller.ui_.setFocus(GameUI::FocusState::MENU);
    controller.ui_.setSelectedMenuItem(1); // 1 is "Undo"
    controller.processInput('\n');

    // The cell should be reverted to its original state (0)
    EXPECT_EQ(controller.board_.getCell(0, 0), 0);
    EXPECT_TRUE(controller.isRunning());

    // Try to undo again when there are no moves left
    // The board's undo() method should return false
    EXPECT_FALSE(controller.board_.undo());
}