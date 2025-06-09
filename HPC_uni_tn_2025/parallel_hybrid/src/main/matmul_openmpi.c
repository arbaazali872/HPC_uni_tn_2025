#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

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

// Perform matrix multiplication for a block of rows using OpenMP
void matmul_block_omp(double *A_block, double *B, double *C_block, int rows_per_proc, int dim) {
    #pragma omp parallel for collapse(2) schedule(static)
    for (int i = 0; i < rows_per_proc; i++) {
        for (int j = 0; j < dim; j++) {
            double sum = 0.0;
            for (int k = 0; k < dim; k++) {
                sum += A_block[i * dim + k] * B[k * dim + j];
            }
            C_block[i * dim + j] = sum;
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

    // Allocate and initialize matrices
    if (rank == 0) {
        A = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
        B = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
        C = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));

        srand(time(NULL));
        generate_random_matrix(A, MATRIX_SIZE, MATRIX_SIZE);
        generate_random_matrix(B, MATRIX_SIZE, MATRIX_SIZE);
    } else {
        B = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
    }

    A_block = malloc(rows_per_proc * MATRIX_SIZE * sizeof(double));
    C_block = malloc(rows_per_proc * MATRIX_SIZE * sizeof(double));

    // Broadcast B and scatter A
    MPI_Bcast(B, MATRIX_SIZE * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(A, rows_per_proc * MATRIX_SIZE, MPI_DOUBLE,
                A_block, rows_per_proc * MATRIX_SIZE, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    // Compute local block matmul
    start_time = MPI_Wtime();
    matmul_block_omp(A_block, B, C_block, rows_per_proc, MATRIX_SIZE);
    end_time = MPI_Wtime();
    local_elapsed = end_time - start_time;

    // Gather result
    MPI_Gather(C_block, rows_per_proc * MATRIX_SIZE, MPI_DOUBLE,
               C, rows_per_proc * MATRIX_SIZE, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    // Find max timing
    MPI_Reduce(&local_elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Hybrid MPI+OpenMP matmul %dx%d took %.6f seconds with %d MPI ranks and %d OpenMP threads per rank\n",
               MATRIX_SIZE, MATRIX_SIZE, max_elapsed, size, omp_get_max_threads());
    }

    // Cleanup
    free(A_block); free(C_block); free(B);
    if (rank == 0) { free(A); free(C); }

    MPI_Finalize();
    return 0;
}
