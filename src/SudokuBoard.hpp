#ifndef SUDOKU_BOARD_HPP
#define SUDOKU_BOARD_HPP

#include <vector>
#include <array>
#include <optional>
#include <random>

class SudokuBoard {
public:
    static constexpr int MAX_HINTS = 3;                 // Maximum number of hints allowed
    static constexpr int SIZE = 9;
    enum class Difficulty { Easy, Medium, Hard };       // Difficulty levels for puzzle generation
    SudokuBoard() noexcept;
    int getCell(int row, int col) const noexcept;
    bool setCell(int row, int col, int value) noexcept;
    bool isValid() const noexcept;
    bool isFull() const noexcept;
    void clear() noexcept;
    bool isPreFilled(int row, int col) const noexcept;
    bool isValidMove(int row, int col, int value) const noexcept;   // check if placing value at (row, col) is valid
    bool solveBoard(std::mt19937& rng) noexcept;
    std::optional<int> getHint(int row, int col, std::mt19937& rng) noexcept;      // get a hint for cell (row, col)
    int getHintsUsed() const noexcept;                // number of hints used  
    // mark cell as pre-filled or not for testing and puzzle generation
    void setPreFilled(int row, int col, bool value) noexcept;                      

private:
    std::vector<std::vector<int>> board_;                   // 9x9 grid
    std::vector<std::vector<bool>> pre_filled_;             // to track original puzzle cells
    
    // helper to check if a position is valid
    bool isValidPosition(int row, int col) const noexcept;
    
    // helper to check if a value is valid
    bool isValidValue(int value) const noexcept;

    int hints_used_ = 0;                  // count of hints used
};

#endif // SUDOKU_BOARD_HPP