// matmul_serial_timed_big.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

// Change this to 10000, 15000, 20000, etc.
#define N 8000

// Generate a random double in [0,1)
static double rand_double() {
    return (double)rand() / RAND_MAX;
}

// Allocate and fill a rows×cols matrix with random values
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

// C = A * B  (A is N×N, B is N×N, C is N×N)
static void matmul_serial(const double *A,
                          const double *B,
                          double *C)
{
    // Standard triple‐loop
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += A[i*N + k] * B[k*N + j];
            }
            C[i*N + j] = sum;
        }
    }
}

int main() {
    // Print problem size
    printf("Serial matmul: N = %d (allocating ~%.2f GB total)\n",
           N, (3.0 * N * N * sizeof(double)) / 1e9);

    // Seed RNG
    srand((unsigned)time(NULL));

    // Allocate & initialize
    double *A = generate_random_matrix(N, N);
    double *B = generate_random_matrix(N, N);
    double *C = malloc((size_t)N * N * sizeof(double));
    if (!C) {
        fprintf(stderr, "Allocation failure for result matrix\n");
        exit(1);
    }

    // Time the multiplication
    struct timeval t_start, t_end;
    gettimeofday(&t_start, NULL);

    matmul_serial(A, B, C);

    gettimeofday(&t_end, NULL);
    double elapsed = (t_end.tv_sec - t_start.tv_sec)
                   + (t_end.tv_usec - t_start.tv_usec) * 1e-6;

    printf("Serial %dx%d matmul took %.6f seconds\n", N, N, elapsed);

    // Cleanup
    free(A);
    free(B);
    free(C);
    return 0;
}

// Serial matmul: N = 8000 (allocating ~1.54 GB total)
// Serial 8000x8000 matmul took 3610.296050 seconds
