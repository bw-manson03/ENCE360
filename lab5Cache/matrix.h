
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


//transpose a square matrix, first parameter is output
void matrixTranspose(double *res, double *a, size_t n);

//Fill a square matrix with zeroes
void zeroMatrix(double *m, size_t n);

// Fill a square matrix with random values between 0 and 1
double *randomMatrix(size_t n);

// Allocate a square matrix of size n
double *allocMatrix(size_t n);

// Compare two square matrices (a and b) for equality within a tolerance (eps)
// Returns: 1 equal, 0 not equal
int compareMatrix(double *a, double *b, size_t n);

//Get the number of seconds difference between two timespec values
double seconds(struct timespec start, struct timespec stop);


// Matrix multiplication of two square matrices (a, b) of size n
// returns square matrix (res) of size n
void matrixMulBasic(double *res, double *a, double *b, size_t n);
void matrixMulTransposed(double *res, double *a, double *b, size_t n);

// Parameter block controlls inner blocking size
void matrixMulBlocked(double *res, double *a, double *b, size_t n, size_t block);
