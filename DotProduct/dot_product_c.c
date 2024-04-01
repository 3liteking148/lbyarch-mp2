#include "dot_product_c.h"

double dot_product_c(double* A, double* B, int n, double* sdot) {
    double result = 0.0;
    int i;

    for (i = 0; i < n; i++) {
        result += A[i] * B[i];
    }

    *sdot = result;
    return result;
}