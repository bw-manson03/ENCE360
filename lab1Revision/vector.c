/*
 * vector.c - an exercise in memory allocation and pointers.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <malloc.h>


// No typedef here! It's worth leaving the struct tag in for the practice; a lot of code is written like this. 
struct Vector {
	double *data;
	size_t size;
};


//
// Initialize a new vector of size n
// Allocate memory and use memset to set the data to zero
//
// Functions you'll likely need (check these with the man pages):
// malloc 
// memset, sizeof
//
struct Vector *newVector(size_t n) {
	// TODO: Implement me
	struct Vector *newVec = malloc(sizeof(struct Vector));
	newVec->data = malloc(n * sizeof(double));
	memset(newVec->data, 0, n * sizeof(double));
	newVec->size = n;

	return newVec;
}


//
// Allocate a vector and copy the data from a pointer 
//
struct Vector *newFrom(double *data, size_t n) {
	struct Vector *v = newVector(n);
	for (size_t i = 0; i < n; ++i) {
		v->data[i] = data[i];
	}

	return v;
}

//
// Take two vectors as input, add their contents and return a new vector with the added data 
// it is an error if the sizes of the input vectors have different sizes
//
struct Vector *addVectors(struct Vector *v1, struct Vector *v2) {
	assert(v1->size == v2->size);

	// TODO: Implement me
	struct Vector *result = newVector(v1->size);
	for(size_t i = 0; i < v1->size; ++i){
		result->data[i] = v1->data[i] + v2->data[i];
	}

	return result;
}


//
// Print out the contents of a vector
//
void printVector(struct Vector *v) {
	printf("<");
	for (size_t i = 0; i < v->size; ++i) {
		printf("%.2f", v->data[i]);

		if (i + 1 < v->size) {
			printf(", ");
		}
	}
	printf(">\n");
}



int main() {
	double values_x[] = { 1.43,2.0,3.5,4.0 };
	double values_y[] = { 7.0,-2.7,4.0,0.0 };

	struct Vector *x = newFrom(values_x, 4);
	struct Vector *y = newFrom(values_y, 4);

	printVector(x);
	printVector(y);


	struct Vector *r = addVectors(x, y);
	printVector(r);

		// Expected output: 
		// <1.43, 2.00, 3.50, 4.00>
		// <7.00, -2.70, 4.00, 0.00>  
		// <8.43, -0.70, 7.50, 4.00>

		// Experiment with different inputs

	return 0;
}



