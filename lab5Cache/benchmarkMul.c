#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "matrix.h"


void benchmarkMul(size_t n, uint32_t repeats) {
	
	double *a = randomMatrix(n);
	double *b = randomMatrix(n); 
	
	double *res = allocMatrix(n);
	
	struct timespec start, stop;
	
	double basic = 0.0;
	
	if (n < 1200) {
		clock_gettime( CLOCK_REALTIME, &start);
		for (uint32_t i = 0; i < repeats; ++i) {
			matrixMulBasic(res, a, b, n);
		}
		
		clock_gettime( CLOCK_REALTIME, &stop);
		basic = seconds(start, stop) / (double)repeats;
	}
	
	

	clock_gettime( CLOCK_REALTIME, &start);
	for (uint32_t i = 0; i < repeats; ++i) {
		matrixMulTransposed(res, a, b, n);
	}
	
	clock_gettime( CLOCK_REALTIME, &stop);
	double transposed = seconds(start, stop) / (double)repeats;
	

	
	clock_gettime( CLOCK_REALTIME, &start);
	for (uint32_t i = 0; i < repeats; ++i) {
		matrixMulBlocked(res, a, b, n, 16);
	}
	
	clock_gettime( CLOCK_REALTIME, &stop);
	double blocked = seconds(start, stop) / (double)repeats;
	
	
	printf("%zu basic: %.5lf, transposed: %.5lf, blocked: %.5lf\n", n, basic, transposed, blocked);
	
	free(a);
	free(b);
	free(res);
}

int main(int argc, char**argv) {
	
	if (argc < 2) {
		printf("usage: ./benchmarkMul sizeIncrement\n");
		exit(1);
	}
	
	int sizeIncr = atoi(argv[1]);
	
	int reps = 2;
	
	for (size_t i = 1; i < 21; ++i) {
		benchmarkMul(i * sizeIncr, reps);
	}

	return 0;
}
