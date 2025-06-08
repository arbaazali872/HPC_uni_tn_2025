# HPC Matrix Multiplication Project

This project implements a serial matrix multiplication program in C. It includes functions for generating random matrices and performing the matrix multiplication.

## Files

- **matmul_serial_8000.c**: Contains the C implementation of the matrix multiplication program.
- **compile_matmul.sh**: A Bash script to compile the `matmul_serial_8000.c` program.

## Compilation Instructions

To compile the program, run the following command in your terminal:

```bash
bash compile_matmul.sh
```

This will generate an executable file for the matrix multiplication program.

## Running the Program

After compiling, you can run the program using the following command:

```bash
./matmul_serial_8000
```

This will execute the matrix multiplication with the defined matrix size.

## Notes

- Ensure you have a C compiler installed (e.g., `gcc`).
- You can modify the matrix size in the `matmul_serial_8000.c` file by changing the `#define N` directive.