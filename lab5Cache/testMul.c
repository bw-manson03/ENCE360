#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "matrix.h"


void testMatrix(size_t n) {
	double *a = randomMatrix(n);
	double *b = randomMatrix(n); 

	double *x = allocMatrix(n);
	double *y = allocMatrix(n);



	matrixMulBasic(x, a, b, n);
	matrixMulTransposed(y, a, b, n);

	printf("matrixMulTransposed: %s\n", compareMatrix(x, y, n) ? "pass" : "fail");

	matrixMulBlocked(y, a, b, n, 16);
	printf("matrixMulBlocked: %s\n", compareMatrix(x, y, n) ? "pass" : "fail");


	free(a);
	free(b);

	free(x);
	free(y);
	
}

int main(int argc, char**argv) {
	
	testMatrix(100);

	return 0;
}