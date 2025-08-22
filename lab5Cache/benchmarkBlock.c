
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "matrix.h"


void benchmarkBlocks(size_t n, int repeats) {
	
	double *a = randomMatrix(n);
	double *b = randomMatrix(n); 
	
	double *res = allocMatrix(n);
	
	struct timespec start, stop;

	
	for (size_t blocks = 4; blocks <= 1024; blocks *= 2) {
		clock_gettime( CLOCK_REALTIME, &start);
		for (size_t i = 0; i < repeats; ++i) {
			matrixMulBlocked(res, a, b, n, blocks);
		}
		
		clock_gettime( CLOCK_REALTIME, &stop);
		double blocked = seconds(start, stop);
		

		printf("%zu, blocks: %zu, blocked: %.4lf\n", n, blocks, blocked);
	}
	
	free(a);
	free(b);
	free(res);
}



int main(int argc, char**argv) {
	


	benchmarkBlocks(1024, 1);

	return 0;
}