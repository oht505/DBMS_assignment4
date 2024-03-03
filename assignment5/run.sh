#!/bin/bash

# Compile main.cpp with g++ using C++11 standard and output the executable as main.out
g++ -std=c++11 main.cpp -o main.out

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running main.out..."
    # Execute the compiled program
    ./main.out
else
    echo "Compilation failed."
fi
