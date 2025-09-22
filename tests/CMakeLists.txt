# Find GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Test executable
add_executable(test_sudokuboard
    test_sudokuboard.cpp
    ../src/SudokuBoard.cpp
)
target_include_directories(test_sudokuboard PRIVATE ../src)
target_link_libraries(test_sudokuboard PRIVATE GTest::GTest GTest::Main)

# Register test
add_test(NAME SudokuBoardTests COMMAND test_sudokuboard)