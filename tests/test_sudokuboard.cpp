#include <gtest/gtest.h>
#include "SudokuBoard.hpp"

// Test fixture for SudokuBoard
class SudokuBoardTest : public ::testing::Test {
protected:
    SudokuBoard board;
};

TEST_F(SudokuBoardTest, Constructor_InitializesEmptyBoard) {
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            EXPECT_EQ(board.getCell(row, col), 0) << "Cell (" << row << "," << col << ") should be empty";
            EXPECT_FALSE(board.isPreFilled(row, col)) << "Cell (" << row << "," << col << ") should not be pre-filled";
        }
    }
    EXPECT_FALSE(board.isFull()) << "New board should not be full";
    EXPECT_TRUE(board.isValid()) << "Empty board should be valid";
}

TEST_F(SudokuBoardTest, GetCell_InvalidPosition_ReturnsZero) {
    EXPECT_EQ(board.getCell(-1, 0), 0) << "Negative row should return 0";
    EXPECT_EQ(board.getCell(0, -1), 0) << "Negative col should return 0";
    EXPECT_EQ(board.getCell(SudokuBoard::SIZE, 0), 0) << "Out-of-bounds row should return 0";
    EXPECT_EQ(board.getCell(0, SudokuBoard::SIZE), 0) << "Out-of-bounds col should return 0";
}

TEST_F(SudokuBoardTest, SetCell_ValidPosition_Succeeds) {
    EXPECT_TRUE(board.setCell(0, 0, 5)) << "Setting valid cell should succeed";
    EXPECT_EQ(board.getCell(0, 0), 5) << "Cell should have value 5";
    EXPECT_TRUE(board.setCell(8, 8, 9)) << "Setting corner cell should succeed";
    EXPECT_EQ(board.getCell(8, 8), 9) << "Cell should have value 9";
}

TEST_F(SudokuBoardTest, SetCell_InvalidPositionOrValue_Fails) {
    EXPECT_FALSE(board.setCell(-1, 0, 1)) << "Negative row should fail";
    EXPECT_FALSE(board.setCell(0, -1, 1)) << "Negative col should fail";
    EXPECT_FALSE(board.setCell(SudokuBoard::SIZE, 0, 1)) << "Out-of-bounds row should fail";
    EXPECT_FALSE(board.setCell(0, SudokuBoard::SIZE, 1)) << "Out-of-bounds col should fail";
    EXPECT_FALSE(board.setCell(0, 0, 10)) << "Value > 9 should fail";
    EXPECT_FALSE(board.setCell(0, 0, -1)) << "Negative value should fail";
}

TEST_F(SudokuBoardTest, IsValid_ValidBoard_ReturnsTrue) {
    // Set a valid partial board
    board.setCell(0, 0, 1);
    board.setCell(1, 1, 2);
    board.setCell(2, 2, 3);
    EXPECT_TRUE(board.isValid()) << "Valid partial board should be valid";
}

TEST_F(SudokuBoardTest, IsValid_ComplexBoard) {
    // Set a valid partial board with multiple values
    board.setCell(0, 0, 1);
    board.setCell(0, 3, 4);
    board.setCell(0, 6, 7);
    board.setCell(3, 0, 2);
    board.setCell(3, 3, 5);
    board.setCell(3, 6, 8);
    board.setCell(6, 0, 3);
    board.setCell(6, 3, 6);
    board.setCell(6, 6, 9);
    EXPECT_TRUE(board.isValid()) << "Complex valid board should be valid";

    // Introduce duplicates in row, column, and box
    board.setCell(0, 1, 1); // Duplicate 1 in row 0
    EXPECT_FALSE(board.isValid()) << "Duplicate in row should be invalid";
    
    board.clear();
    board.setCell(0, 0, 1);
    board.setCell(1, 0, 1); // Duplicate 1 in column 0
    EXPECT_FALSE(board.isValid()) << "Duplicate in column should be invalid";
    
    board.clear();
    board.setCell(0, 0, 1);
    board.setCell(1, 1, 1); // Duplicate 1 in box 0
    EXPECT_FALSE(board.isValid()) << "Duplicate in box should be invalid";
}

TEST_F(SudokuBoardTest, IsFull_EmptyAndPartialBoard_ReturnsFalse) {
    EXPECT_FALSE(board.isFull()) << "Empty board should not be full";
    board.setCell(0, 0, 1);
    EXPECT_FALSE(board.isFull()) << "Partial board should not be full";
}

TEST_F(SudokuBoardTest, IsFull_FullBoard_ReturnsTrue) {
    // Fill entire board (simplified valid board)
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            board.setCell(row, col, (row + col) % 9 + 1);
        }
    }
    EXPECT_TRUE(board.isFull()) << "Full board should return true";
}

TEST_F(SudokuBoardTest, Clear_ResetsBoardAndPreFilled) {
    board.setCell(0, 0, 5);
    board.clear();
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            EXPECT_EQ(board.getCell(row, col), 0) << "Cleared board should have zeros";
            EXPECT_FALSE(board.isPreFilled(row, col)) << "Cleared board should have no pre-filled cells";
        }
    }
}

TEST_F(SudokuBoardTest, IsPreFilled_InitiallyFalse) {
    EXPECT_FALSE(board.isPreFilled(0, 0)) << "New board should have no pre-filled cells";
    EXPECT_FALSE(board.isPreFilled(8, 8)) << "Corner cell should not be pre-filled";
    EXPECT_FALSE(board.isPreFilled(-1, 0)) << "Invalid position should return false";
}