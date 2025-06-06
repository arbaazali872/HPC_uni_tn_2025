// matmul_serial_timed_big_outer.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

// Change this to 10000, 15000, 20000, etc.
#define N 9000

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

// Outer-product matrix multiplication: C += outer(A[:,k], B[k,:])
static void matmul_outer(const double *A,
                         const double *B,
                         double *C) 
{
    // Initialize C to zero
    for (long long i = 0; i < (long long)N * N; i++) {
        C[i] = 0.0;
    }

    // Outer-product triple loop: for each k, update the rank-1 contribution
    for (int k = 0; k < N; k++) {
        for (int i = 0; i < N; i++) {
            double a_ik = A[i * N + k];
            // If a_ik is zero, skip the inner loop, but here values are random
            for (int j = 0; j < N; j++) {
                C[i * N + j] += a_ik * B[k * N + j];
            }
        }
    }
}

int main() {
    // Print problem size
    printf("Serial matmul (outer): N = %d (allocating ~%.2f GB total)\n",
           N, (3.0 * N * N * sizeof(double)) / 1e9);

    // Seed RNG
    srand((unsigned)time(NULL));

    // Allocate & initialize A, B
    double *A = generate_random_matrix(N, N);
    double *B = generate_random_matrix(N, N);

    // Allocate result matrix C
    double *C = malloc((size_t)N * N * sizeof(double));
    if (!C) {
        fprintf(stderr, "Allocation failure for result matrix\n");
        exit(1);
    }

    // Time the outer-product multiplication
    struct timeval t_start, t_end;
    gettimeofday(&t_start, NULL);

    matmul_outer(A, B, C);

    gettimeofday(&t_end, NULL);
    double elapsed = (t_end.tv_sec - t_start.tv_sec)
                   + (t_end.tv_usec - t_start.tv_usec) * 1e-6;

    printf("Serial (outer) %dx%d matmul took %.6f seconds\n", N, N, elapsed);

    // Cleanup
    free(A);
    free(B);
    free(C);
    return 0;
}

// Serial matmul (outer): N = 9000 (allocating ~1.94 GB total)
// Serial (outer) 9000x9000 matmul took 921.716736 seconds