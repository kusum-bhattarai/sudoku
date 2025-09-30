#!/bin/bash

# Navigate to build directory, create if it doesn't exist
mkdir -p build
cd build

# Run CMake to configure the project
cmake ..

# Build the project
make

# Run tests with verbose output
./sudoku

# Return to project root
cd ..