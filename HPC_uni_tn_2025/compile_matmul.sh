#!/bin/bash

# Compile the matmul_serial_8000.c program
gcc -o matmul_serial_8000 matmul_serial_8000.c -lm

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Executable created: matmul_serial_8000"
else
    echo "Compilation failed."
fi
