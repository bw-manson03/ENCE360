#define _POSIX_C_SOURCE 200809L // Needed for home PC as running mimimal POSIX
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <signal.h>

// PI constant if not defined by math.h
// I was having issues with M_PI not being defined by default on home system.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Process management initialization
#define MAX_CHILDREN 4
sem_t numFreeChildren;


// MATH FUNCTIONS AND INTEGRATION
typedef double MathFunc_t(double);

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
double integrateTrap(MathFunc_t* func, double rangeStart, double rangeEnd, size_t numSteps)
{
	double rangeSize = rangeEnd - rangeStart;
	double dx = rangeSize / numSteps;

	double sum = 0;
	double area = 0;
	for (size_t i = 0; i < numSteps; i++) {
		double smallx = rangeStart + i*dx;
		double bigx = rangeStart + (i+1)*dx;

		sum += ( func(smallx) + func(bigx) ) / 2; // Would be more efficient to multiply area by dx once at the end. 
	}
	area = sum * dx;
	return area;
}

// INPUT HANDLING
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

// PROCESS MANAGEMENT
void handleChildExit(int sigNum) {
    (void)sigNum;
    int status;
    pid_t pid;

    // Reap all exited children
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        sem_post(&numFreeChildren);
        printf("[Parent] Child %d finished. Slot freed.\n", pid);
    }
}

// MAIN PROGRAM
int main(void) {
    // Initialize semaphore with MAX_CHILDREN slots
    if (sem_init(&numFreeChildren, 0, MAX_CHILDREN) != 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    // Register persistent SIGCHLD handler
    struct sigaction sa;
    sa.sa_handler = handleChildExit;
    sigemptyset(&sa.sa_mask);
	// Restart syscalls and only catch terminated children
	// Without SA_RESTART, multiple children were created but only one was reaped
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) != 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    double rangeStart, rangeEnd;
    size_t numSteps;
    MathFunc_t* func;
    char funcName[10] = {'\0'};

    printf("Query format: [func] [start] [end] [numSteps]\n");

    while (true) {
        sem_wait(&numFreeChildren);  // Wait for an available child slot

        if (!getValidInput(&func, funcName, &rangeStart, &rangeEnd, &numSteps))
            break;

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            sem_post(&numFreeChildren);
            continue;
        }

        if (pid == 0) {
            // Child process
            double area = integrateTrap(func, rangeStart, rangeEnd, numSteps);
            printf("[Child %d] Integral of \"%s\" from %g to %g = %.10g\n",
                   getpid(), funcName, rangeStart, rangeEnd, area);
            _exit(0);
        } else {
            // Parent process
            printf("[Parent] Spawned child %d\n", pid);
        }
    }

    // Wait for any remaining children before exiting
    int status;
	pid_t pid;
    while ((pid = wait(&status)) > 0) {
    	sem_post(&numFreeChildren);
    	printf("[Parent] Child %d finished. Slot freed.\n", pid);
    	fflush(stdout);
	}
    _exit(0); // Forces more immediate exit than normal - **Use this to exit processes throughout the assignment!**
}