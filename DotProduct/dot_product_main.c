#include <stdio.h>
#include <windows.h>

#define N 2000

// 2^20 1048576
// 2^24 16777216
// 2^28 268435456
// 2^29 536870912
// 2^30 1073741824

extern void dot_product_asm(double* A, double* B, size_t n, double* sdot);

double dot_product_c(double* A, double* B, int n, double* sdot) {
	double result = 0.0;
	int i;

	for (i = 0; i < n; i++) {
		result += A[i] * B[i];
	}

	*sdot = result;
	return result;
}

int main() {
	printf("Initializing %ld elements (requires %lf GiB of memory).\n", N, ((N * 2 * sizeof(double)) / 1024.0 / 1024.0 / 1024.0));
	double* A = (double*)_aligned_malloc(N * sizeof(double), 32);
	double* B = (double*)_aligned_malloc(N * sizeof(double), 32);
	if (A == NULL || B == NULL) {
		printf("Error allocating memory.\n");
		exit(1);
	}

	for (int i = 0; i < N; i++) {
		A[i] = i;
		B[i] = i;
	}

	LARGE_INTEGER start, end, frequency;
	for (int i = 0; i < 10; i++) {
		double out;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start);
		dot_product_asm(A, B, N, &out);
		QueryPerformanceCounter(&end);

		double time_ms = (end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
		printf("dp=%lf \n", out);
		printf("compute time : % lfms \n", time_ms);
	}

	_aligned_free(A); // Free allocated memory
	_aligned_free(B);

	return 0;

}