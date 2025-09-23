enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
FetchContent_MakeAvailable(googletest)

# Test executable
add_executable(test_sudokuboard
    test_sudokuboard.cpp
    ../src/SudokuBoard.cpp
)
target_include_directories(test_sudokuboard PRIVATE ../src)
target_link_libraries(test_sudokuboard PRIVATE GTest::gtest GTest::gtest_main)

# Register test
add_test(NAME SudokuBoardTests COMMAND test_sudokuboard)