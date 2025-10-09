#include <gtest/gtest.h>
#include "SudokuBoard.hpp"
#include <algorithm>

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

TEST_F(SudokuBoardTest, IsValidMove_CorrectBehavior) {
    // Set up a partial board
    board.setCell(0, 0, 1); // Row 0, col 0 has 1
    board.setCell(1, 1, 2); // Box (0,0), row 1, col 1 has 2
    board.setCell(3, 0, 3); // Col 0, row 3 has 3

    // Valid moves
    EXPECT_TRUE(board.isValidMove(0, 1, 4)) << "Value 4 should be valid in row 0, col 1 (no conflicts)";
    EXPECT_TRUE(board.isValidMove(2, 2, 5)) << "Value 5 should be valid in row 2, col 2 (no conflicts)";
    EXPECT_TRUE(board.isValidMove(8, 8, 9)) << "Value 9 should be valid in row 8, col 8 (no conflicts)";

    // Invalid moves
    EXPECT_FALSE(board.isValidMove(0, 2, 1)) << "Value 1 in row 0 should fail (duplicate in row)";
    EXPECT_FALSE(board.isValidMove(2, 0, 3)) << "Value 3 in col 0 should fail (duplicate in col)";
    EXPECT_FALSE(board.isValidMove(2, 2, 2)) << "Value 2 in box (0,0) should fail (duplicate in box)";
    
    // Edge cases
    EXPECT_FALSE(board.isValidMove(-1, 0, 1)) << "Negative row should fail";
    EXPECT_FALSE(board.isValidMove(0, -1, 1)) << "Negative col should fail";
    EXPECT_FALSE(board.isValidMove(SudokuBoard::SIZE, 0, 1)) << "Out-of-bounds row should fail";
    EXPECT_FALSE(board.isValidMove(0, SudokuBoard::SIZE, 1)) << "Out-of-bounds col should fail";
    EXPECT_FALSE(board.isValidMove(0, 0, 0)) << "Value 0 should fail";
    EXPECT_FALSE(board.isValidMove(0, 0, 10)) << "Value 10 should fail";
}

TEST_F(SudokuBoardTest, SolveBoard_ProducesValidFullBoard) {
    std::random_device rd;
    std::mt19937 rng(rd());
    EXPECT_TRUE(board.solveBoard(rng)) << "Empty board should be solvable";
    EXPECT_TRUE(board.isFull()) << "Solved board should be full";
    EXPECT_TRUE(board.isValid()) << "Solved board should be valid";
    // Check no pre-filled cells were set (solver doesn't touch pre_filled_)
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            EXPECT_FALSE(board.isPreFilled(row, col)) << "Solver should not set pre-filled cells";
        }
    }
}

TEST_F(SudokuBoardTest, GetHint_CorrectBehavior) {
    std::random_device rd;
    std::mt19937 rng(rd());

    // Set up a partial board
    board.setCell(0, 0, 1);
    board.setCell(1, 1, 2);
    board.setCell(3, 0, 3);
    board.setPreFilled(0, 0, true); // Mark some cells as pre-filled
    board.setPreFilled(1, 1, true);

    // Valid hint request
    auto hint = board.getHint(0, 1, rng);
    ASSERT_TRUE(hint.has_value()) << "Hint for valid cell should return a value";
    EXPECT_TRUE(*hint >= 1 && *hint <= 9) << "Hint should be 1-9";
    EXPECT_TRUE(board.isValidMove(0, 1, *hint)) << "Hint should be valid for (0,1)";
    EXPECT_EQ(board.getHintsUsed(), 1) << "Hints used should be 1";

    // Invalid requests
    EXPECT_FALSE(board.getHint(-1, 0, rng).has_value()) << "Negative row should return nullopt";
    EXPECT_FALSE(board.getHint(0, -1, rng).has_value()) << "Negative col should return nullopt";
    EXPECT_FALSE(board.getHint(SudokuBoard::SIZE, 0, rng).has_value()) << "Out-of-bounds row should return nullopt";
    EXPECT_FALSE(board.getHint(0, 0, rng).has_value()) << "Pre-filled cell should return nullopt";

    // Max hints limit
    board.getHint(0, 2, rng); // Hint 2
    board.getHint(0, 3, rng); // Hint 3
    EXPECT_EQ(board.getHintsUsed(), 3) << "Hints used should be 3";
    EXPECT_FALSE(board.getHint(0, 4, rng).has_value()) << "Exceeding max hints should return nullopt";
}

TEST_F(SudokuBoardTest, RemoveCells_ProducesSolvablePuzzle) {
    std::random_device rd;
    std::mt19937 rng(rd());

    // Create a full valid board
    board.solveBoard(rng);
    ASSERT_TRUE(board.isFull()) << "Board should be full before removing cells";

    // Remove 41 cells (Easy: 40 left)
    int removed = board.removeCells(41, rng);
    int filled = 0;
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            if (board.getCell(row, col) != 0) {
                ++filled;
            }
        }
    }
    EXPECT_EQ(filled, 40) << "Should have 40 filled cells after removing 41";
    EXPECT_EQ(removed, 41) << "Should have removed 41 cells";
    EXPECT_TRUE(board.isValid()) << "Board should be valid after removal";
    SudokuBoard temp = board;
    EXPECT_TRUE(temp.solveBoard(rng)) << "Board should be solvable after removal";
}

TEST_F(SudokuBoardTest, GeneratePuzzle_ProducesValidPuzzle) {
    std::random_device rd;
    std::mt19937 rng(rd());

    // Test Easy (40 cells)
    board.generatePuzzle(SudokuBoard::Difficulty::Easy);
    int filled = 0;
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            if (board.getCell(row, col) != 0) {
                ++filled;
                EXPECT_TRUE(board.isPreFilled(row, col)) << "Filled cell should be pre-filled";
            } else {
                EXPECT_FALSE(board.isPreFilled(row, col)) << "Empty cell should not be pre-filled";
            }
        }
    }
    EXPECT_EQ(filled, 40) << "Easy puzzle should have 40 filled cells";
    EXPECT_TRUE(board.isValid()) << "Easy puzzle should be valid";
    SudokuBoard temp_easy = board;
    EXPECT_TRUE(temp_easy.solveBoard(rng)) << "Easy puzzle should be solvable";
    EXPECT_EQ(board.getHintsUsed(), 0) << "New puzzle should reset hints";

    // Test Medium (25 cells)
    board.generatePuzzle(SudokuBoard::Difficulty::Medium);
    filled = 0;
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            if (board.getCell(row, col) != 0) {
                ++filled;
                EXPECT_TRUE(board.isPreFilled(row, col)) << "Filled cell should be pre-filled";
            } else {
                EXPECT_FALSE(board.isPreFilled(row, col)) << "Empty cell should not be pre-filled";
            }
        }
    }
    EXPECT_EQ(filled, 25) << "Medium puzzle should have 25 filled cells";
    EXPECT_TRUE(board.isValid()) << "Medium puzzle should be valid";
    SudokuBoard temp_medium = board;
    EXPECT_TRUE(temp_medium.solveBoard(rng)) << "Medium puzzle should be solvable";

    // Test Hard (15 cells)
    board.generatePuzzle(SudokuBoard::Difficulty::Hard);
    filled = 0;
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            if (board.getCell(row, col) != 0) {
                ++filled;
                EXPECT_TRUE(board.isPreFilled(row, col)) << "Filled cell should be pre-filled";
            } else {
                EXPECT_FALSE(board.isPreFilled(row, col)) << "Empty cell should not be pre-filled";
            }
        }
    }
    EXPECT_EQ(filled, 15) << "Hard puzzle should have 15 filled cells";
    EXPECT_TRUE(board.isValid()) << "Hard puzzle should be valid";
    SudokuBoard temp_hard = board;
    EXPECT_TRUE(temp_hard.solveBoard(rng)) << "Hard puzzle should be solvable";
}

TEST_F(SudokuBoardTest, GeneratePuzzle_Randomization) {
    std::random_device rd;
    std::mt19937 rng(rd());

    // Generate two Easy puzzles
    board.generatePuzzle(SudokuBoard::Difficulty::Easy);
    std::vector<std::vector<int>> board1 = board.getBoard(); 
    board.generatePuzzle(SudokuBoard::Difficulty::Easy);
    std::vector<std::vector<int>> board2 = board.getBoard(); 
    // Check if boards differ
    bool different = false;
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            if (board1[row][col] != board2[row][col]) {
                different = true;
                break;
            }
        }
        if (different) break;
    }
    EXPECT_TRUE(different) << "Two Easy puzzles should differ due to randomization";
}

TEST_F(SudokuBoardTest, GeneratePuzzle_ResetsPartialBoard) {
    std::random_device rd;
    std::mt19937 rng(rd());

    // Set user moves
    board.setCell(0, 0, 1);
    board.setCell(1, 1, 2);
    board.setPreFilled(0, 0, false); // User move, not pre-filled

    // Generate Easy puzzle
    board.generatePuzzle(SudokuBoard::Difficulty::Easy);
    int filled = 0;
    for (int row = 0; row < SudokuBoard::SIZE; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE; ++col) {
            if (board.getCell(row, col) != 0) {
                ++filled;
                EXPECT_TRUE(board.isPreFilled(row, col)) << "Filled cell should be pre-filled";
            } else {
                EXPECT_FALSE(board.isPreFilled(row, col)) << "Empty cell should not be pre-filled";
            }
        }
    }
    EXPECT_EQ(filled, 40) << "Easy puzzle should have 40 filled cells after reset";
    EXPECT_TRUE(board.isValid()) << "Puzzle should be valid";
    EXPECT_EQ(board.getHintsUsed(), 0) << "Puzzle should reset hints";
}

TEST_F(SudokuBoardTest, Undo_CorrectBehavior) {
    std::random_device rd;
    std::mt19937 rng(rd());

    // Generate a puzzle
    board.generatePuzzle(SudokuBoard::Difficulty::Easy);
    
    // Find non-pre-filled cells (first 5 empty cells)
    std::vector<std::pair<int, int>> empty_cells;
    for (int row = 0; row < SudokuBoard::SIZE && empty_cells.size() < 5; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE && empty_cells.size() < 5; ++col) {
            if (board.getCell(row, col) == 0 && !board.isPreFilled(row, col)) {
                empty_cells.emplace_back(row, col);
            }
        }
    }
    ASSERT_GE(empty_cells.size(), 5) << "Not enough empty cells for testing";

    // Make user move
    EXPECT_TRUE(board.setCell(empty_cells[0].first, empty_cells[0].second, 4)) << "Setting valid cell should succeed";
    EXPECT_EQ(board.getCell(empty_cells[0].first, empty_cells[0].second), 4) << "Cell should be 4";
    EXPECT_TRUE(board.canUndo()) << "Should be able to undo";
    
    // Undo first move
    EXPECT_TRUE(board.undo()) << "Undo should succeed";
    EXPECT_EQ(board.getCell(empty_cells[0].first, empty_cells[0].second), 0) << "Cell should be restored to 0";
    EXPECT_FALSE(board.canUndo()) << "No more moves to undo";

    // Test max moves (5)
    EXPECT_TRUE(board.setCell(empty_cells[0].first, empty_cells[0].second, 1)) << "Set first cell to 1";
    EXPECT_TRUE(board.setCell(empty_cells[1].first, empty_cells[1].second, 2)) << "Set second cell to 2";
    EXPECT_TRUE(board.setCell(empty_cells[2].first, empty_cells[2].second, 3)) << "Set third cell to 3";
    EXPECT_TRUE(board.setCell(empty_cells[3].first, empty_cells[3].second, 4)) << "Set fourth cell to 4";
    EXPECT_TRUE(board.setCell(empty_cells[4].first, empty_cells[4].second, 5)) << "Set fifth cell to 5";
    EXPECT_TRUE(board.setCell(empty_cells[0].first, empty_cells[0].second, 6)) << "Set first cell to 6 (pops first move)";
    EXPECT_EQ(board.getCell(empty_cells[0].first, empty_cells[0].second), 6) << "First cell should be 6";
    EXPECT_TRUE(board.canUndo()) << "Should be able to undo";
    EXPECT_TRUE(board.undo()) << "Undo 6th move";
    EXPECT_EQ(board.getCell(empty_cells[0].first, empty_cells[0].second), 1) << "First cell should restore to 1 (value before 6th move)";
    EXPECT_TRUE(board.undo()) << "Undo 5th move";
    EXPECT_EQ(board.getCell(empty_cells[4].first, empty_cells[4].second), 0) << "Fifth cell should be 0";
    EXPECT_TRUE(board.undo()) << "Undo 4th move";
    EXPECT_EQ(board.getCell(empty_cells[3].first, empty_cells[3].second), 0) << "Fourth cell should be 0";
    EXPECT_TRUE(board.undo()) << "Undo 3rd move";
    EXPECT_EQ(board.getCell(empty_cells[2].first, empty_cells[2].second), 0) << "Third cell should be 0";
    EXPECT_TRUE(board.undo()) << "Undo 2nd move";
    EXPECT_EQ(board.getCell(empty_cells[1].first, empty_cells[1].second), 0) << "Second cell should be 0";
    EXPECT_FALSE(board.canUndo()) << "No more moves to undo after 5";

    // Test undo on pre-filled cell
    board.setPreFilled(0, 0, true);
    EXPECT_FALSE(board.setCell(0, 0, 5)) << "Setting pre-filled cell should fail";
    EXPECT_FALSE(board.undo()) << "Undo should fail with no new moves";
}

TEST_F(SudokuBoardTest, Undo_AdditionalCases) {
    std::random_device rd;
    std::mt19937 rng(rd());

    // Generate a puzzle
    board.generatePuzzle(SudokuBoard::Difficulty::Easy);
    
    // Find non-pre-filled cells (first 6 empty cells)
    std::vector<std::pair<int, int>> empty_cells;
    for (int row = 0; row < SudokuBoard::SIZE && empty_cells.size() < 6; ++row) {
        for (int col = 0; col < SudokuBoard::SIZE && empty_cells.size() < 6; ++col) {
            if (board.getCell(row, col) == 0 && !board.isPreFilled(row, col)) {
                empty_cells.emplace_back(row, col);
            }
        }
    }
    ASSERT_GE(empty_cells.size(), 6) << "Not enough empty cells for testing";

    // to test multiple undos on same cell
    EXPECT_TRUE(board.setCell(empty_cells[0].first, empty_cells[0].second, 1)) << "Set cell to 1";
    EXPECT_TRUE(board.setCell(empty_cells[0].first, empty_cells[0].second, 2)) << "Set same cell to 2";
    EXPECT_TRUE(board.setCell(empty_cells[0].first, empty_cells[0].second, 3)) << "Set same cell to 3";
    EXPECT_EQ(board.getCell(empty_cells[0].first, empty_cells[0].second), 3) << "Cell should be 3";
    EXPECT_TRUE(board.canUndo()) << "Should be able to undo";
    EXPECT_TRUE(board.undo()) << "Undo to 2";
    EXPECT_EQ(board.getCell(empty_cells[0].first, empty_cells[0].second), 2) << "Cell should be 2";
    EXPECT_TRUE(board.undo()) << "Undo to 1";
    EXPECT_EQ(board.getCell(empty_cells[0].first, empty_cells[0].second), 1) << "Cell should be 1";
    EXPECT_TRUE(board.undo()) << "Undo to 0";
    EXPECT_EQ(board.getCell(empty_cells[0].first, empty_cells[0].second), 0) << "Cell should be 0";
    EXPECT_FALSE(board.canUndo()) << "No more moves to undo";

    // to test invalid undo attempts
    EXPECT_FALSE(board.undo()) << "Undo should fail when moves_ is empty";
    EXPECT_FALSE(board.setCell(-1, 0, 1)) << "Invalid position should fail";
    EXPECT_FALSE(board.undo()) << "Undo should fail after invalid setCell";
    EXPECT_FALSE(board.setCell(0, 0, 10)) << "Invalid value should fail";
    EXPECT_FALSE(board.undo()) << "Undo should fail after invalid value";

    // to test undo after clear
    EXPECT_TRUE(board.setCell(empty_cells[1].first, empty_cells[1].second, 4)) << "Set cell to 4";
    EXPECT_TRUE(board.canUndo()) << "Should be able to undo";
    board.clear();
    EXPECT_FALSE(board.canUndo()) << "No moves after clear";
    EXPECT_FALSE(board.undo()) << "Undo should fail after clear";

    // to test max moves with mixed cells
    EXPECT_TRUE(board.setCell(empty_cells[0].first, empty_cells[0].second, 1)) << "Set cell 0 to 1";
    EXPECT_TRUE(board.setCell(empty_cells[1].first, empty_cells[1].second, 2)) << "Set cell 1 to 2";
    EXPECT_TRUE(board.setCell(empty_cells[2].first, empty_cells[2].second, 3)) << "Set cell 2 to 3";
    EXPECT_TRUE(board.setCell(empty_cells[3].first, empty_cells[3].second, 4)) << "Set cell 3 to 4";
    EXPECT_TRUE(board.setCell(empty_cells[4].first, empty_cells[4].second, 5)) << "Set cell 4 to 5";
    EXPECT_TRUE(board.setCell(empty_cells[5].first, empty_cells[5].second, 6)) << "Set cell 5 to 6 (pops cell 0)";
    EXPECT_EQ(board.getCell(empty_cells[5].first, empty_cells[5].second), 6) << "Cell 5 should be 6";
    EXPECT_TRUE(board.undo()) << "Undo cell 5 to 0";
    EXPECT_EQ(board.getCell(empty_cells[5].first, empty_cells[5].second), 0) << "Cell 5 should be 0";
    EXPECT_TRUE(board.undo()) << "Undo cell 4 to 0";
    EXPECT_EQ(board.getCell(empty_cells[4].first, empty_cells[4].second), 0) << "Cell 4 should be 0";
    EXPECT_TRUE(board.undo()) << "Undo cell 3 to 0";
    EXPECT_EQ(board.getCell(empty_cells[3].first, empty_cells[3].second), 0) << "Cell 3 should be 0";
    EXPECT_TRUE(board.undo()) << "Undo cell 2 to 0";
    EXPECT_EQ(board.getCell(empty_cells[2].first, empty_cells[2].second), 0) << "Cell 2 should be 0";
    EXPECT_TRUE(board.undo()) << "Undo cell 1 to 0";
    EXPECT_EQ(board.getCell(empty_cells[1].first, empty_cells[1].second), 0) << "Cell 1 should be 0";
    EXPECT_FALSE(board.canUndo()) << "No more moves to undo";
}

TEST_F(SudokuBoardTest, FindErrors_IdentifiesAllDuplicates) {
    // Row duplicate
    board.setCell(0, 1, 5);
    board.setCell(0, 8, 5);

    // Column duplicate
    board.setCell(1, 2, 3);
    board.setCell(7, 2, 3);

    // Box duplicate
    board.setCell(4, 4, 8);
    board.setCell(5, 5, 8);
    
    // A cell that is part of multiple errors (row and box)
    board.setCell(2, 0, 9); // This cell is part of two conflicts
    board.setCell(2, 5, 9); // Row conflict
    board.setCell(1, 1, 9); // Box conflict

    auto errors = board.findErrors();
    
    // Use a set for easy lookup, as order doesn't matter
    std::set<std::pair<int, int>> error_set(errors.begin(), errors.end());

    // Check that all expected error cells are present
    EXPECT_TRUE(error_set.count({0, 1}));
    EXPECT_TRUE(error_set.count({0, 8}));
    EXPECT_TRUE(error_set.count({1, 2}));
    EXPECT_TRUE(error_set.count({7, 2}));
    EXPECT_TRUE(error_set.count({4, 4}));
    EXPECT_TRUE(error_set.count({5, 5}));
    EXPECT_TRUE(error_set.count({2, 0}));
    EXPECT_TRUE(error_set.count({2, 5}));
    EXPECT_TRUE(error_set.count({1, 1}));

    // Check total number of unique error cells
    EXPECT_EQ(error_set.size(), 9);
}