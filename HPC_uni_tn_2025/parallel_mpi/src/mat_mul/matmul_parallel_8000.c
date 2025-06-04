#include "../common/matmul.h"
#include "mpi.h"

#define N 1000  // Change to 8000, 10000 etc. as needed

static double rand_double() {
    return (double)rand() / RAND_MAX;
}

static double* generate_random_matrix(int rows, int cols) {
    double *A = malloc((size_t)rows * cols * sizeof(double));
    if (!A) {
        fprintf(stderr, "Allocation failure for %dx%d matrix\n", rows, cols);
        exit(1);
    }
    for (long long i = 0; i < (long long)rows * cols; i++) {
        A[i] = rand_double();
    }
    return A;
}

// Perform matrix multiplication for a block of rows
void matmul_block(const double *A_block, const double *B, double *C_block,
                  int rows_per_proc, int N) {
    for (int i = 0; i < rows_per_proc; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += A_block[i * N + k] * B[k * N + j];
            }
            C_block[i * N + j] = sum;
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size;


    MPI_Init(&argc, &argv);

    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0) {
        if (rank == 0) {
            fprintf(stderr, "N must be divisible by number of processes\n");
        }
        MPI_Finalize();
        return 1;
    }

    int rows_per_proc = N / size;

    double *A = NULL;
    double *B = NULL;
    double *C = NULL;

    if (rank == 0) {
        printf("MPI matmul: N = %d, processes = %d\n", N, size);
        printf("Allocating ~%.2f GB total\n", (3.0 * N * N * sizeof(double)) / 1e9);

        srand((unsigned)time(NULL));
        A = generate_random_matrix(N, N);
        B = generate_random_matrix(N, N);
        C = malloc((size_t)N * N * sizeof(double));
    } else {
        B = malloc((size_t)N * N * sizeof(double));
    }

    double *A_block = malloc((size_t)rows_per_proc * N * sizeof(double));
    double *C_block = malloc((size_t)rows_per_proc * N * sizeof(double));

    // Scatter rows of A to all processes
    MPI_Scatter(A, rows_per_proc * N, MPI_DOUBLE,
                A_block, rows_per_proc * N, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    // Broadcast B to all processes
    MPI_Bcast(B, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Start timing
    double t_start = MPI_Wtime();

    // Perform local multiplication
    matmul_block(A_block, B, C_block, rows_per_proc, N);

    // End timing
    double t_end = MPI_Wtime();
    double local_elapsed = t_end - t_start;

    // Gather all C blocks back to the root process
    MPI_Gather(C_block, rows_per_proc * N, MPI_DOUBLE,
               C, rows_per_proc * N, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("MPI %dx%d matmul took %.6f seconds\n", N, N, local_elapsed);
        free(A);
        free(C);
    }

    free(B);
    free(A_block);
    free(C_block);

    MPI_Finalize();
    return 0;
}
