// matmul_openmp_outer.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>

// Change N as needed (e.g., 10000, 15000, 20000)
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

// Outer‐product matrix multiplication with OpenMP:
// C[i][j] += A[i][k] * B[k][j] for all k, i, j
static void matmul_openmp_outer(const double *A,
                                const double *B,
                                double *C)
{
    // 1) Zero‐initialize C in parallel
    #pragma omp parallel for
    for (long long idx = 0; idx < (long long)N * N; idx++) {
        C[idx] = 0.0;
    }

    // 2) Perform outer‐product accumulation in parallel
    //    Parallelize over k and i via collapse(2)
    #pragma omp parallel for collapse(2) schedule(static)
    for (int k = 0; k < N; k++) {
        for (int i = 0; i < N; i++) {
            double a_ik = A[i * N + k];
            for (int j = 0; j < N; j++) {
                C[i * N + j] += a_ik * B[k * N + j];
            }
        }
    }
}

int main() {
    // 1) Print problem size and memory footprint
    printf("OpenMP outer‐product matmul: N = %d (allocating ~%.2f GB total)\n",
           N, (3.0 * N * N * sizeof(double)) / 1e9);

    // 2) Seed RNG
    srand((unsigned)time(NULL));

    // 3) Allocate & initialize A and B
    double *A = generate_random_matrix(N, N);
    double *B = generate_random_matrix(N, N);

    // 4) Allocate result matrix C
    double *C = malloc((size_t)N * N * sizeof(double));
    if (!C) {
        fprintf(stderr, "Allocation failure for result matrix\n");
        exit(1);
    }

    // 5) Time the OpenMP outer‐product multiplication
    struct timeval t_start, t_end;
    gettimeofday(&t_start, NULL);

    matmul_openmp_outer(A, B, C);

    gettimeofday(&t_end, NULL);
    double elapsed = (t_end.tv_sec - t_start.tv_sec)
                   + (t_end.tv_usec - t_start.tv_usec) * 1e-6;

    // 6) Print timing and thread count
    printf("OpenMP outer %dx%d matmul took %.6f seconds (threads = %d)\n",
           N, N, elapsed, omp_get_max_threads());

    // 7) Cleanup
    free(A);
    free(B);
    free(C);
    return 0;
}

// OpenMP outerΓÇÉproduct matmul: N = 9000 (allocating ~1.94 GB total)
// OpenMP outer 9000x9000 matmul took 578.969921 seconds (threads = 8)

// OpenMP outerΓÇÉproduct matmul: N = 8000 (allocating ~1.54 GB total)
// OpenMP outer 8000x8000 matmul took 389.126284 seconds (threads = 8)
