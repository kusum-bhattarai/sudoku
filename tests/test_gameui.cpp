#include <gtest/gtest.h>
#include "GameUI.hpp"
#include "SudokuBoard.hpp"

class GameUITest : public ::testing::Test {
protected:
    SudokuBoard board;
    GameUI ui{board};
};

TEST_F(GameUITest, Constructor_InitializesBoardReference) {
    EXPECT_EQ(&ui.getBoard(), &board.getBoard()) << "GameUI should hold correct board reference";
}

TEST_F(GameUITest, DisplayBoard_EmptyBoard_NoPreFilled) {
    board.clear();
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            EXPECT_FALSE(board.isPreFilled(row, col)) << "Empty board should have no pre-filled cells";
        }
    }
    // manual test: Run ./sudoku with board.clear(), expect all spaces, no blue cells
}

TEST_F(GameUITest, DisplayBoard_EasyPuzzle_PreFilledCount) {
    board.generatePuzzle(SudokuBoard::Difficulty::Easy);
    int pre_filled_count = 0;
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            if (board.isPreFilled(row, col)) {
                ++pre_filled_count;
            }
        }
    }
    EXPECT_EQ(pre_filled_count, 40) << "Easy puzzle should have 40 pre-filled cells";
    // manual test: Run ./sudoku, verify ~40 blue numbers, grid aligned, A-I/1-9 labels
}

TEST_F(GameUITest, DisplayBoard_FullBoard_AllValues) {
    board.clear();
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            board.setCell(row, col, (row + col) % 9 + 1);
            board.setPreFilled(row, col, false); // Ensure no pre-filled
        }
    }
    EXPECT_TRUE(board.isFull()) << "Board should be full";
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            EXPECT_FALSE(board.isPreFilled(row, col)) << "Full board should have no pre-filled cells";
        }
    }
    // manual test: Run ./sudoku, expect all cells filled (1-9), no blue
}

TEST_F(GameUITest, DisplayBoard_SomePreFilled) {
    board.clear();
    board.setCell(0, 0, 5);
    board.setPreFilled(0, 0, true);
    board.setCell(8, 8, 9);
    board.setPreFilled(8, 8, true);
    EXPECT_TRUE(board.isPreFilled(0, 0)) << "Cell (0,0) should be pre-filled";
    EXPECT_TRUE(board.isPreFilled(8, 8)) << "Cell (8,8) should be pre-filled";
    // manual test: Run ./sudoku, expect 5 and 9 in blue at (0,0) and (8,8), others empty
}