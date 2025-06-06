#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>

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

// Outer‐product matrix multiplication with OpenMP
static void matmul_openmp_outer(const double *A,
                                const double *B,
                                double *C,
                                int N)
{
    // Zero‐initialize C in parallel
    #pragma omp parallel for
    for (long long idx = 0; idx < (long long)N * N; idx++) {
        C[idx] = 0.0;
    }

    // Perform outer‐product accumulation in parallel
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

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <matrix_dimension>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "Invalid matrix dimension: %s\n", argv[1]);
        return 1;
    }

    printf("OpenMP outer-product matmul: N = %d (allocating ~%.2f GB total)\n",
           N, (3.0 * N * N * sizeof(double)) / 1e9);

    srand((unsigned)time(NULL));

    double *A = generate_random_matrix(N, N);
    double *B = generate_random_matrix(N, N);
    double *C = malloc((size_t)N * N * sizeof(double));
    if (!C) {
        fprintf(stderr, "Allocation failure for result matrix\n");
        exit(1);
    }

    struct timeval t_start, t_end;
    gettimeofday(&t_start, NULL);

    matmul_openmp_outer(A, B, C, N);

    gettimeofday(&t_end, NULL);
    double elapsed = (t_end.tv_sec - t_start.tv_sec)
                   + (t_end.tv_usec - t_start.tv_usec) * 1e-6;

    printf("OpenMP outer %dx%d matmul took %.6f seconds (threads = %d)\n",
           N, N, elapsed, omp_get_max_threads());

    free(A);
    free(B);
    free(C);
    return 0;
}
