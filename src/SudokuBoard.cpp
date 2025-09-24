#include "SudokuBoard.hpp"

SudokuBoard::SudokuBoard() noexcept {
    // initialize 9x9 board with zeros
    board_ = std::vector<std::vector<int>>(SIZE, std::vector<int>(SIZE, 0));

    // all false for the prefilled tracker at initialization
    pre_filled_ = std::vector<std::vector<bool>>(SIZE, std::vector<bool>(SIZE, false));
}

int SudokuBoard::getCell(int row, int col) const noexcept {
    if (!isValidPosition(row, col)) {
        return 0; 
    }
    return board_[row][col];
}

bool SudokuBoard::setCell(int row, int col, int value) noexcept {
    if (!isValidPosition(row, col) || !isValidValue(value) || pre_filled_[row][col]) {
        return false;   // cant set the value for the cell (invalid position/ value/ pre-filled cell)
    }
    board_[row][col] = value;
    return true;
}

bool SudokuBoard::isValid() const noexcept {
    // Trackers for rows, columns, and 3x3 boxes 
    std::array<std::array<bool, SIZE + 1>, SIZE> rows{};
    std::array<std::array<bool, SIZE + 1>, SIZE> cols{};
    std::array<std::array<bool, SIZE + 1>, SIZE> boxes{};

    // single pass through the board 
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            int val = board_[row][col];
            if (val == 0) continue; 

            // check row for duplicates
            if (rows[row][val]) return false; 
            rows[row][val] = true;

            // check col for duplicates
            if (cols[col][val]) return false; 
            cols[col][val] = true;

            // check 3x3 box for duplicates
            int box_idx = (row / 3) * 3 + col / 3; 
            if (boxes[box_idx][val]) return false; 
            boxes[box_idx][val] = true;
        }
    }
    return true;
}

bool SudokuBoard::isFull() const noexcept {
    for (const auto& row : board_) {
        for (int val : row) {
            if (val == 0) return false;     // found an empty cell
        }
    }
    return true;
}

void SudokuBoard::clear() noexcept {
    for (auto& row : board_) {
        std::fill(row.begin(), row.end(), 0);
    }
    for (auto& row : pre_filled_) {
        std::fill(row.begin(), row.end(), false);
    }
}

bool SudokuBoard::isPreFilled(int row, int col) const noexcept {
    if (!isValidPosition(row, col)) {
        return false;
    }
    return pre_filled_[row][col];
}

bool SudokuBoard::isValidPosition(int row, int col) const noexcept {
    return row >= 0 && row < SIZE && col >= 0 && col < SIZE;
}

bool SudokuBoard::isValidValue(int value) const noexcept {
    return value >= 0 && value <= 9; // 0 = empty, 1-9 = valid numbers
}

bool SudokuBoard::isValidMove(int row, int col, int value) const noexcept {
    if (!isValidPosition(row, col) || !isValidValue(value) || value == 0) {
        return false;
    }
    // Check row
    for (int c = 0; c < SIZE; ++c) {
        if (c != col && board_[row][c] == value) return false;
    }
    // Check column
    for (int r = 0; r < SIZE; ++r) {
        if (r != row && board_[r][col] == value) return false;
    }
    // Check 3x3 box
    int box_row = (row / 3) * 3;
    int box_col = (col / 3) * 3;
    for (int r = box_row; r < box_row + 3; ++r) {
        for (int c = box_col; c < box_col + 3; ++c) {
            if ((r != row || c != col) && board_[r][c] == value) return false;
        }
    }
    return true;
}

bool SudokuBoard::solveBoard(std::mt19937& rng) noexcept {
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            if (board_[row][col] == 0) {
                // try numbers 1-9 in random order
                std::vector<int> values = {1, 2, 3, 4, 5, 6, 7, 8, 9};
                std::shuffle(values.begin(), values.end(), rng);
                for (int value : values) {
                    if (isValidMove(row, col, value)) {
                        board_[row][col] = value;
                        if (solveBoard(rng)) return true;
                        board_[row][col] = 0; // backtrack
                    }
                }
                return false; // no valid number found
            }
        }
    }
    return true; // board solved
}

std::optional<int> SudokuBoard::getHint(int row, int col, std::mt19937& rng) noexcept {
    if (!isValidPosition(row, col) || pre_filled_[row][col] || hints_used_ >= MAX_HINTS) {
        return std::nullopt; // Invalid position, pre-filled, or max hints reached
    }

    // Try values 1-9 in random order
    std::vector<int> values = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::shuffle(values.begin(), values.end(), rng);
    for (int value : values) {
        if (isValidMove(row, col, value)) {
            // Check if this value leads to a solvable board
            int original = board_[row][col];
            board_[row][col] = value;
            SudokuBoard temp = *this; // Copy to test solvability
            if (temp.solveBoard(rng)) {
                board_[row][col] = original; // Restore original state
                ++hints_used_;
                return value;
            }
            board_[row][col] = original; // Restore if unsolvable
        }
    }
    return std::nullopt; // No valid hint found
}

int SudokuBoard::getHintsUsed() const noexcept {
    return hints_used_;
}

void SudokuBoard::setPreFilled(int row, int col, bool value) noexcept {
    if (isValidPosition(row, col)) {
        pre_filled_[row][col] = value;
    }
}

// helper to remove cells while ensuring unique solution
int SudokuBoard::removeCells(int to_remove, std::mt19937& rng) noexcept {
    std::vector<std::pair<int, int>> cells;
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            if (board_[row][col] != 0) {
                cells.emplace_back(row, col);
            }
        }
    }
    std::shuffle(cells.begin(), cells.end(), rng);
    
    int removed = 0;
    for (const auto& [row, col] : cells) {
        if (removed >= to_remove) break;
        int backup = board_[row][col];
        board_[row][col] = 0;
        SudokuBoard temp = *this;
        if (temp.solveBoard(rng)) {
            ++removed;
        } else {
            board_[row][col] = backup;
        }
    }
    return removed;
}

void SudokuBoard::generatePuzzle(Difficulty difficulty) noexcept {
    // Clear board and reset hints
    clear();
    
    // Initialize random number generator
    std::random_device rd;
    std::mt19937 rng(rd());
    
    // Generate full valid board
    solveBoard(rng);
    
    // Remove cells based on difficulty
    int to_remove = 0;
    switch (difficulty) {
        case Difficulty::Easy:   to_remove = 41; break; // 40 cells remain
        case Difficulty::Medium: to_remove = 56; break; // 25 cells remain
        case Difficulty::Hard:   to_remove = 66; break; // 15 cells remain
    }
    removeCells(to_remove, rng);
    
    // Mark remaining cells as pre-filled
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            pre_filled_[row][col] = (board_[row][col] != 0);
        }
    }
}