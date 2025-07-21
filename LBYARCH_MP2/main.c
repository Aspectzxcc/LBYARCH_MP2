#include <stdio.h>    // for printf
#include <stdlib.h>   // for malloc, free, rand, srand
#include <math.h>     // for fabsf, log2
#include <time.h>     // for time (to seed rand)

// include the newly created C kernel header
#include "kernel.h"

// platform-specific headers for high-resolution timing
#ifdef _WIN32
#include <windows.h>  // for QueryPerformanceCounter, QueryPerformanceFrequency
#else
#include <time.h>     // for clock_gettime, CLOCK_MONOTONIC (POSIX)
#endif

// constants
#define NUM_RUNS 30     // number of times to run each kernel for averaging
#define EPSILON 1e-6f   // tolerance for floating-point comparisons in correctness check

// function prototypes

// x86-64 assembly kernel: same functionality as C kernel
extern void calculate_distance_asm(long long n, const float* X1, const float* X2, const float* Y1, const float* Y2, float* Z);

// helper function to initialize vectors with random float values
void initialize_vectors(long long n, float* X1, float* X2, float* Y1, float* Y2);

int main() {
    // array of vector sizes (n) to test
    // 1LL << 20 = 2^20
    // 1LL << 24 = 2^24
    // 1LL << 28 = 2^28
    long long vector_sizes[] = {
        1LL << 20,
        1LL << 24,
        1LL << 28
    };
    int num_vector_sizes = sizeof(vector_sizes) / sizeof(vector_sizes[0]);

    // pointers for input and output vectors
    float* X1 = NULL, * X2 = NULL, * Y1 = NULL, * Y2 = NULL;
    float* Z_c = NULL, * Z_asm = NULL; // separate output buffers for C and ASM kernels

    // timing variables
#ifdef _WIN32
    LARGE_INTEGER start_time, end_time, frequency;
    QueryPerformanceFrequency(&frequency); // get CPU frequency for timing calculation
#else
    struct timespec start_time, end_time;
#endif

    // seed the random number generator for vector initialization
    srand((unsigned int)time(NULL));

    printf("Starting Euclidean Distance Kernel Performance Test...\n");
    printf("---------------------------------------------------\n\n");

    for (int i = 0; i < num_vector_sizes; ++i) {
        long long n = vector_sizes[i]; // current vector length
        printf("Testing with N = %lld (2^%d elements)\n", n, (int)log2(n));

        // calculate required memory in bytes for one array
        size_t array_size_bytes = (size_t)n * sizeof(float);

        // check for potential integer overflow if n is extremely large
        if (array_size_bytes / sizeof(float) != n) {
            printf("Error: Memory allocation size calculation overflowed for N = %lld. Skipping.\n\n", n);
            continue;
        }

        // allocate memory for all vectors
        X1 = (float*)malloc(array_size_bytes);
        X2 = (float*)malloc(array_size_bytes);
        Y1 = (float*)malloc(array_size_bytes);
        Y2 = (float*)malloc(array_size_bytes);
        Z_c = (float*)malloc(array_size_bytes); // output for C kernel
        Z_asm = (float*)malloc(array_size_bytes); // output for Assembly kernel

        // check if all memory allocations were successful
        if (!X1 || !X2 || !Y1 || !Y2 || !Z_c || !Z_asm) {
            fprintf(stderr, "Error: Failed to allocate memory for N = %lld. Please check available RAM. Skipping.\n\n", n);
            // free any already allocated memory before continuing to next size
            free(X1); free(X2); free(Y1); free(Y2); free(Z_c); free(Z_asm);
            X1 = X2 = Y1 = Y2 = Z_c = Z_asm = NULL; // set to NULL defensively
            continue;
        }

        // initialize input vectors with random data
        initialize_vectors(n, X1, X2, Y1, Y2);

        double total_time_c = 0.0;
        double total_time_asm = 0.0;

        printf("  Running %d times for averaging...\n", NUM_RUNS);
        for (int run = 0; run < NUM_RUNS; ++run) {
            // time C kernel execution
#ifdef _WIN32
            QueryPerformanceCounter(&start_time);
#else
            clock_gettime(CLOCK_MONOTONIC, &start_time);
#endif
            calculate_distance_c(n, X1, X2, Y1, Y2, Z_c); // call C kernel
#ifdef _WIN32
            QueryPerformanceCounter(&end_time);
            total_time_c += (double)(end_time.QuadPart - start_time.QuadPart) * 1000.0 / frequency.QuadPart; // milliseconds
#else
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            total_time_c += (end_time.tv_sec - start_time.tv_sec) * 1000.0 + (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0; // milliseconds
#endif

            // time assembly kernel execution
#ifdef _WIN32
            QueryPerformanceCounter(&start_time);
#else
            clock_gettime(CLOCK_MONOTONIC, &start_time);
#endif
            calculate_distance_asm(n, X1, X2, Y1, Y2, Z_asm); // call assembly kernel
#ifdef _WIN32
            QueryPerformanceCounter(&end_time);
            total_time_asm += (double)(end_time.QuadPart - start_time.QuadPart) * 1000.0 / frequency.QuadPart; // milliseconds
#else
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            total_time_asm += (end_time.tv_sec - start_time.tv_sec) * 1000.0 + (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0; // milliseconds
#endif
        }

        // display average execution times
        printf("  Average C Kernel Time:   %.4f ms\n", total_time_c / NUM_RUNS);
        printf("  Average ASM Kernel Time: %.4f ms\n", total_time_asm / NUM_RUNS);

        // correctness check
        // compare the output of the assembly kernel against the C kernel
        int errors = 0;
        for (long long j = 0; j < n; ++j) {
            if (fabsf(Z_c[j] - Z_asm[j]) > EPSILON) {
                errors++;
            }
        }

        if (errors == 0) {
            printf("  Correctness Check: PASS (ASM output matches C output within tolerance)\n");
        }
        else {
            printf("  Correctness Check: FAIL (%d discrepancies found)\n", errors);
        }

        // display first 10 elements of Z
        printf("  First 10 elements of Z (C version):\n  [");
        for (int k = 0; k < (n > 10 ? 10 : (int)n); ++k) {
            printf("%.6f%s", Z_c[k], (k < 9 && k < n - 1) ? ", " : "");
        }
        printf("]\n");

        printf("  First 10 elements of Z (ASM version):\n  [");
        for (int k = 0; k < (n > 10 ? 10 : (int)n); ++k) {
            printf("%.6f%s", Z_asm[k], (k < 9 && k < n - 1) ? ", " : "");
        }
        printf("]\n\n");

        // free memory for the current vector size
        free(X1); free(X2); free(Y1); free(Y2); free(Z_c); free(Z_asm);
        X1 = X2 = Y1 = Y2 = Z_c = Z_asm = NULL; // nullify pointers after freeing
    }

    printf("---------------------------------------------------\n");
    printf("Performance Test Complete.\n");

    return 0;
}

// helper function: initialize vectors
// fills the input vectors with random single-precision float values
void initialize_vectors(long long n, float* X1, float* X2, float* Y1, float* Y2) {
    for (long long i = 0; i < n; ++i) {
        // generate random floats between 0.0 and 100.0
        X1[i] = (float)rand() / RAND_MAX * 100.0f;
        X2[i] = (float)rand() / RAND_MAX * 100.0f;
        Y1[i] = (float)rand() / RAND_MAX * 100.0f;
        Y2[i] = (float)rand() / RAND_MAX * 100.0f;
    }
}