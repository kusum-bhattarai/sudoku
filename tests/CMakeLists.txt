enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
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

add_executable(test_gameui
    ../src/SudokuBoard.cpp
    ../src/GameUI.cpp
    test_gameui.cpp
)
target_include_directories(test_gameui PRIVATE ../src)
target_link_libraries(test_gameui PRIVATE GTest::gtest GTest::gtest_main ncurses)

add_test(NAME GameUITests COMMAND test_gameui)
