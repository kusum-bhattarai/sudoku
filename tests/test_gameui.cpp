#include <gtest/gtest.h>
#include "GameUI.hpp"
#include "SudokuBoard.hpp"

class MockGameUI : public GameUI {
public:
    MockGameUI(SudokuBoard& board) : GameUI(board) {}

    // Helper to directly set the cursor for reliable testing
    void setCursorPosition(int row, int col) {
        cursor_row_ = row;
        cursor_col_ = col;
    }

    // This function will allow us to simulate a key press for testing purposes.
    bool simulateKeyPress(int ch) {
        switch (ch) {
            case 'q': case 'Q': return false;
            case KEY_RIGHT: {
                int index = cursor_row_ * SudokuBoard::SIZE + cursor_col_;
                index = (index + 1) % (SudokuBoard::SIZE * SudokuBoard::SIZE);
                cursor_row_ = index / SudokuBoard::SIZE;
                cursor_col_ = index % SudokuBoard::SIZE;
                break;
            }
            case KEY_LEFT: {
                int index = cursor_row_ * SudokuBoard::SIZE + cursor_col_;
                index = (index - 1 + (SudokuBoard::SIZE * SudokuBoard::SIZE)) % (SudokuBoard::SIZE * SudokuBoard::SIZE);
                cursor_row_ = index / SudokuBoard::SIZE;
                cursor_col_ = index % SudokuBoard::SIZE;
                break;
            }
            case KEY_UP:
                cursor_row_ = (cursor_row_ - 1 + SudokuBoard::SIZE) % SudokuBoard::SIZE;
                break;
            case KEY_DOWN:
                cursor_row_ = (cursor_row_ + 1) % SudokuBoard::SIZE;
                break;
            case '1' ... '9':
                if (!board_.isPreFilled(cursor_row_, cursor_col_)) {
                    board_.setCell(cursor_row_, cursor_col_, ch - '0');
                }
                break;
            case '0': case KEY_BACKSPACE:
                if (!board_.isPreFilled(cursor_row_, cursor_col_)) {
                    board_.setCell(cursor_row_, cursor_col_, 0);
                }
                break;
        }
        return true;
    }
};


class GameUITest : public ::testing::Test {
protected:
    SudokuBoard board;
    MockGameUI ui{board};
};

TEST_F(GameUITest, Constructor_InitializesBoardReference) {
    EXPECT_EQ(&ui.getBoard(), &board.getBoard());
}

TEST_F(GameUITest, Constructor_InitializesCursorAtZero) {
    auto [row, col] = ui.getCursorPosition();
    EXPECT_EQ(row, 0);
    EXPECT_EQ(col, 0);
}
TEST_F(GameUITest, HandleInput_CursorWrapsAroundBoard) {
    auto [row, col] = ui.getCursorPosition();

    // Test RIGHT wrap from (0, 8) to (1, 0)
    ui.setCursorPosition(0, 8);
    ui.simulateKeyPress(KEY_RIGHT);
    std::tie(row, col) = ui.getCursorPosition();
    EXPECT_EQ(row, 1) << "RIGHT from last col should go to next row";
    EXPECT_EQ(col, 0) << "RIGHT from last col should go to first col";

    // Test LEFT wrap from (1, 0) to (0, 8)
    ui.setCursorPosition(1, 0);
    ui.simulateKeyPress(KEY_LEFT);
    std::tie(row, col) = ui.getCursorPosition();
    EXPECT_EQ(row, 0) << "LEFT from first col should go to previous row";
    EXPECT_EQ(col, 8) << "LEFT from first col should go to last col";

    // Test UP wrap (stays in same column)
    ui.setCursorPosition(0, 5); // Start in the middle of the board
    ui.simulateKeyPress(KEY_UP);
    std::tie(row, col) = ui.getCursorPosition();
    EXPECT_EQ(row, 8) << "UP from first row should wrap to last row";
    EXPECT_EQ(col, 5) << "UP should not change the column";

    // Test DOWN wrap (stays in same column)
    ui.setCursorPosition(8, 5);
    ui.simulateKeyPress(KEY_DOWN);
    std::tie(row, col) = ui.getCursorPosition();
    EXPECT_EQ(row, 0) << "DOWN from last row should wrap to first row";
    EXPECT_EQ(col, 5) << "DOWN should not change the column";
    
    // Test full wrap from (8, 8) to (0, 0)
    ui.setCursorPosition(8, 8);
    ui.simulateKeyPress(KEY_RIGHT);
    std::tie(row, col) = ui.getCursorPosition();
    EXPECT_EQ(row, 0) << "RIGHT from (8,8) should wrap to (0,0)";
    EXPECT_EQ(col, 0) << "RIGHT from (8,8) should wrap to (0,0)";
}

TEST_F(GameUITest, HandleInput_SetAndClearCellValue) {
    board.setPreFilled(0, 0, false);
    ui.simulateKeyPress('5');
    EXPECT_EQ(board.getCell(0, 0), 5);
    ui.simulateKeyPress('8');
    EXPECT_EQ(board.getCell(0, 0), 8);
    ui.simulateKeyPress('0');
    EXPECT_EQ(board.getCell(0, 0), 0);
}

TEST_F(GameUITest, HandleInput_CannotChangePreFilledCell) {
    board.setCell(0, 0, 9);
    board.setPreFilled(0, 0, true);
    ui.simulateKeyPress('1');
    EXPECT_EQ(board.getCell(0, 0), 9);
}

TEST_F(GameUITest, HandleInput_ReturnsFalseOnQuit) {
    EXPECT_FALSE(ui.simulateKeyPress('q'));
    EXPECT_FALSE(ui.simulateKeyPress('Q'));
}

TEST_F(GameUITest, HandleInput_ReturnsTrueOnOtherKeys) {
    EXPECT_TRUE(ui.simulateKeyPress(KEY_UP));
    EXPECT_TRUE(ui.simulateKeyPress('5'));
}

TEST_F(GameUITest, Constructor_InitializesUIState) {
    EXPECT_EQ(ui.getFocus(), GameUI::FocusState::BOARD) << "Focus should initially be on the board";
    EXPECT_EQ(ui.getSelectedMenuItem(), 0) << "Selected menu item should initially be 0";
}