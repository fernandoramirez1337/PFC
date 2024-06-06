#!/bin/bash

# Run the Python script
echo "Generating 2MB data in data.bin..."
python util.py

# Compile and build the GIFT128 test
echo "Compiling google_tests_GIFT128.cpp..."
g++ -o test_gift google_tests_GIFT128.cpp -lgtest -lgtest_main -pthread GIFT/GIFT.c GIFT/GIFT.h

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation of google_tests_GIFT128.cpp failed."
    exit 1
fi

# Compile and build the SAND128 test
echo "Compiling google_tests_SAND128.cpp..."
g++ -o test_sand google_tests_SAND128.cpp -lgtest -lgtest_main -pthread SAND/SAND.c SAND/SAND.h

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation of google_tests_SAND128.cpp failed."
    exit 1
fi

echo "Compilation successful."

# Optionally, you can run the tests after compiling
echo "Running test_gift..."
./test_gift

echo "Running test_sand..."
./test_sand

echo "All tasks completed."
