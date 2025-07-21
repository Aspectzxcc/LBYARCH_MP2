#ifndef KERNEL_H
#define KERNEL_H

// C Kernel: Calculates Z[i] = sqrt((X2[i] - X1[i])^2 + (Y2[i] - Y1[i])^2)
// Inputs: n (vector length), X1, X2, Y1, Y2 (input vectors)
// Output: Z (result vector)
void calculate_distance_c(long long n, const float* X1, const float* X2, const float* Y1, const float* Y2, float* Z);

#endif // KERNEL_H