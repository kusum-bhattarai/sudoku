cmake_minimum_required(VERSION 3.20)
project(SudokuGame CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find ncurses (required for UI)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Main executable
add_executable(sudoku
    src/main.cpp
    src/SudokuBoard.cpp
)
target_include_directories(sudoku PRIVATE src)
target_link_libraries(sudoku PRIVATE ${CURSES_LIBRARIES})

# Enable testing
enable_testing()
add_subdirectory(tests)