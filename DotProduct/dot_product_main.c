#include <stdio.h>
#include <windows.h>

#define N 1073741824

// 2^20 1048576
// 2^24 16777216
// 2^28 268435456
// 2^29 536870912
// 2^30 1073741824

extern void dot_product_asm(double* A, double* B, size_t n, double* sdot);
extern void dot_product_fma(double* A, double* B, size_t n, double* sdot);
extern void dot_product_fma_avx2(double* A, double* B, size_t n, double* sdot);

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
		A[i] = i/64;
		B[i] = i/64;
	}

	double out = 0;
	printf("Initial correctness check: (mantissa-exponent representation in parenthesis)\n");
	dot_product_asm(A, B, N, &out);
	printf("ASM:      %lf (%lA)\n", out, out);
	dot_product_c(A, B, N, &out);
	printf("C:        %lf (%lA)\n", out, out);
	dot_product_fma(A, B, N, &out);
	printf("FMA:      %lf (%lA)\n", out, out);
	dot_product_fma_avx2(A, B, N, &out);
	printf("FMA-AVX2: %lf (%lA)\n", out, out);
	printf("\n");

	printf("NOTE: output values of FMA/FMA-AVX2 might slightly differ due to different rounding and/or order of operations\n\n");
	
	LARGE_INTEGER start, end, frequency;
	double asm_time_avg = 0;
	double c_time_avg = 0;
	printf("Starting ASM/C test\n");
	printf("%2s | %40s | %10s | %40s | %10s | %6s\n", "#", "ASM output", "Time(ms)", "C output", "Time(ms)", "Equal?");
	for (int i = 0; i < 30; i++) {
		double out = 0;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start);
		dot_product_asm(A, B, N, &out);
		QueryPerformanceCounter(&end);

		double asm_time = (end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
		asm_time_avg += asm_time;
		double asm_result = out;

		out = 0;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start);
		dot_product_c(A, B, N, &out);
		QueryPerformanceCounter(&end);

		double c_time = (end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
		c_time_avg += c_time;
		double c_result = out;

		printf_s("%2d | %40lf | %10lf | %40lf | %10lf | %6c\n", i + 1, asm_result, asm_time, c_result, c_time, asm_result == c_result ? 'Y' : 'N');
	}
	printf("\n");

	printf("Starting FMA/FMA-AVX2 test\n");
	printf("%2s | %40s | %10s | %40s | %10s\n", "#", "FMA output", "Time(ms)", "FMA-AVX2 output", "Time(ms)");
	double fma_time_avg = 0;
	double fmavx_time_avg = 0;
	for (int i = 0; i < 30; i++) {
		double out = 0;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start);
		dot_product_fma(A, B, N, &out);
		QueryPerformanceCounter(&end);

		double fma_time = (end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
		fma_time_avg += fma_time;
		double fma_result = out;

		out = 0;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start);
		dot_product_fma_avx2(A, B, N, &out);
		QueryPerformanceCounter(&end);

		double fma_avx2_time = (end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
		fmavx_time_avg += fma_avx2_time;
		double fma_avx2_result = out;

		printf("%2d | %40lf | %10lf | %40lf | %10lf\n", i + 1, fma_result, fma_time, fma_avx2_result, fma_avx2_time);
	}
	printf("\n");

	_aligned_free(A); // Free allocated memory
	_aligned_free(B);

	printf("average asm time : % lfms \n", asm_time_avg/30);
	printf("average c time : % lfms \n", c_time_avg/30);
	printf("average fma time : % lfms \n", fma_time_avg / 30);
	printf("average fmavx time : % lfms \n", fmavx_time_avg / 30);

	return 0;

}