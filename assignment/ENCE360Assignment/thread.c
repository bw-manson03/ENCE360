#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

// PI constant if not defined by math.h
// I was having issues with M_PI not being defined by default on home system.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define THREADS 7

typedef double MathFunc_t(double);

typedef struct {
    MathFunc_t* func;
    double start;
    double end;
    size_t steps;
} ThreadArgs_t;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
double total = 0.0;


double gaussian(double x)
{
	return exp(-(x*x)/2) / (sqrt(2 * M_PI));
}


double chargeDecay(double x)
{
	if (x < 0) {
		return 0;
	} else if (x < 1) {
		return 1 - exp(-5*x);
	} else {
		return exp(-(x-1));
	}
}

// Integrate using the trapezoid method. 
void* integrateTrap(void* arg)
{
    ThreadArgs_t* args = (ThreadArgs_t*)arg;
	double rangeSize = args->end - args->start;
	double dx = rangeSize / args->steps;

    double sum = 0;
	double area = 0;
    
	for (size_t i = 0; i < args->steps; i++) {
		double smallx = args->start + i*dx;
		double bigx = args->start + (i+1)*dx;

		sum += (args->func(smallx) + args->func(bigx)) / 2;
	}
	area = sum * dx;

    pthread_mutex_lock(&mutex);
    total += area;
    pthread_mutex_unlock(&mutex);

	return NULL;
}

bool getValidInput(MathFunc_t** func, char* funcName, double* start, double* end, size_t* numSteps)
{
	memset(funcName, '\0', 10); // Clear funcName. Magic number used because format strings are annoying. 

	// Read input numbers and place them in the given addresses:
	size_t numRead = scanf("%9s %lf %lf %zu", funcName, start, end, numSteps);

	if (strcmp(funcName, "sin") == 0) {
		*func = &sin;
	} else if (strcmp(funcName, "gauss") == 0) {
		*func = &gaussian;
	} else if (strcmp(funcName, "decay") == 0) {
		*func = &chargeDecay;
	} else {
		*func = NULL;
	}

	// Return whether the given func and range is valid:
	return (numRead == 4 && *func != NULL && *end >= *start && *numSteps > 0);
}

int main(void)
{
	double rangeStart;
	double rangeEnd;
	size_t numSteps;
	MathFunc_t* func;
	char funcName[10] = {'\0'};

	printf("Query format: [func] [start] [end] [numSteps]\n");

	while (getValidInput(&func, funcName, &rangeStart, &rangeEnd, &numSteps)) {
        pthread_t threads[THREADS];
        ThreadArgs_t threadArgs[THREADS];
        total = 0.0; // Reset total for each new calculation

        double totalRange = rangeEnd - rangeStart;
        size_t baseSteps = numSteps / THREADS;
        size_t remainderSteps = numSteps % THREADS;

        double currentStart = rangeStart;

        for (int i = 0; i < THREADS; i++) {
            size_t steps = baseSteps + (i < remainderSteps ? 1 : 0);
            double currentEnd = currentStart + (totalRange * (double)steps / numSteps);
            threadArgs[i].func = func;
            threadArgs[i].start = currentStart;
            threadArgs[i].end = currentEnd;
            threadArgs[i].steps = steps;

            if (pthread_create(&threads[i], NULL, integrateTrap, &threadArgs[i]) != 0) {
                perror("create");
                exit(1);
            }
            currentStart = currentEnd;
        }

        for (int i = 0; i < THREADS; i++) {
            pthread_join(threads[i], NULL);
        }

		printf("The integral of function \"%s\" in range %g to %g is %.10g\n", funcName, rangeStart, rangeEnd, total);
	}

	_exit(0); // Forces more immediate exit than normal - **Use this to exit processes throughout the assignment!**
}
