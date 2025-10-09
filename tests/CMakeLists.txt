enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

# --- Test for SudokuBoard ---
add_executable(test_sudokuboard
    test_sudokuboard.cpp
    ../src/SudokuBoard.cpp
)
target_include_directories(test_sudokuboard PRIVATE ../src)
target_link_libraries(test_sudokuboard PRIVATE GTest::gtest GTest::gtest_main)
add_test(NAME SudokuBoardTests COMMAND test_sudokuboard)

# --- Test for GameUI ---
add_executable(test_gameui
    test_gameui.cpp
    ../src/GameUI.cpp
    ../src/SudokuBoard.cpp
)
target_include_directories(test_gameui PRIVATE ../src)
target_link_libraries(test_gameui PRIVATE GTest::gtest GTest::gtest_main ncurses)
add_test(NAME GameUITests COMMAND test_gameui)

# --- Test for GameController ---
add_executable(test_gamecontroller
    test_gamecontroller.cpp
    ../src/GameController.cpp
    ../src/GameUI.cpp
    ../src/SudokuBoard.cpp
)
target_include_directories(test_gamecontroller PRIVATE ../src)
target_link_libraries(test_gamecontroller PRIVATE GTest::gtest GTest::gtest_main ncurses)
add_test(NAME GameControllerTests COMMAND test_gamecontroller)


# Discover all tests
include(GoogleTest)
gtest_discover_tests(test_sudokuboard)
gtest_discover_tests(test_gameui)
gtest_discover_tests(test_gamecontroller)