#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

  
#ifndef MATRIX_SIZE
#define MATRIX_SIZE 1000
#endif


// Generate a random double between 0 and 1
double rand_double() {
    return (double)rand() / RAND_MAX;
}

// Fill a matrix with random values
void generate_random_matrix(double *matrix, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = rand_double();
    }
}

void matmul_block(double *A_block, double *B, double *C_block, int rows_per_proc, int dim) {
    // Step 1: Zero-initialize the result block
    #pragma omp parallel for
    for (int idx = 0; idx < rows_per_proc * dim; idx++) {
        C_block[idx] = 0.0;
    }

    // Step 2: Outer-product based multiplication
    // Collapse k and i so that threads work on full combinations
    #pragma omp parallel for collapse(2) schedule(static)
    for (int k = 0; k < dim; k++) {
        for (int i = 0; i < rows_per_proc; i++) {
            double a_ik = A_block[i * dim + k];  // ith row of A_block, kth column
            for (int j = 0; j < dim; j++) {
                C_block[i * dim + j] += a_ik * B[k * dim + j];
            }
        }
    }
}


int main(int argc, char *argv[]) {
    int rank, size;
    double *A = NULL, *B = NULL, *C = NULL;
    double *A_block, *C_block;
    double start_time, end_time, local_elapsed, max_elapsed;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (MATRIX_SIZE % size != 0) {
        if (rank == 0) {
            fprintf(stderr, "Error: MATRIX_SIZE (%d) must be divisible by number of processes (%d)\n", MATRIX_SIZE, size);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int rows_per_proc = MATRIX_SIZE / size;

    // Allocate memory for A, B, C
    if (rank == 0) {
        A = (double *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
        B = (double *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
        C = (double *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));

        srand(time(NULL));
        generate_random_matrix(A, MATRIX_SIZE, MATRIX_SIZE);
        generate_random_matrix(B, MATRIX_SIZE, MATRIX_SIZE);
    } else {
        B = (double *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
    }

    A_block = (double *)malloc(rows_per_proc * MATRIX_SIZE * sizeof(double));
    C_block = (double *)malloc(rows_per_proc * MATRIX_SIZE * sizeof(double));

    // Broadcast B and scatter A blocks
    MPI_Bcast(B, MATRIX_SIZE * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(A, rows_per_proc * MATRIX_SIZE, MPI_DOUBLE, A_block, rows_per_proc * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Compute local matrix multiplication
    start_time = MPI_Wtime();
    matmul_block(A_block, B, C_block, rows_per_proc, MATRIX_SIZE);
    end_time = MPI_Wtime();

    local_elapsed = end_time - start_time;

    // Gather result into C
    MPI_Gather(C_block, rows_per_proc * MATRIX_SIZE, MPI_DOUBLE, C, rows_per_proc * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Compute max elapsed time among all processes
    MPI_Reduce(&local_elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("MPI %dx%d matrix multiplication took %.6f seconds (max across ranks)\n", MATRIX_SIZE, MATRIX_SIZE, max_elapsed);
    }

    // Cleanup
    free(A_block);
    free(C_block);
    free(B);
    if (rank == 0) {
        free(A);
        free(C);
    }

    MPI_Finalize();
    return 0;
}
