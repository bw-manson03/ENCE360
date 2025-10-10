#define _POSIX_C_SOURCE 200809L // Needed for home PC as running mimimal POSIX
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <signal.h>

// PI constant if not defined by math.h
// I was having issues with M_PI not being defined by default on home system.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef double MathFunc_t(double);

// Thread management initialization
#define THREADS 4

typedef struct {
    MathFunc_t* func;
    double start;
    double end;
    size_t steps;
	double* total;
} ThreadArgs_t;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Process management initialization
#define MAX_CHILDREN 4

sem_t numFreeChildren;

// MATH FUNCTIONS AND INTEGRATION
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
    *(args->total) += area;
    pthread_mutex_unlock(&mutex);

	return NULL;
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
            pthread_t threads[THREADS];
			ThreadArgs_t threadArgs[THREADS];
			double total = 0.0; // Reset total for each new calculation

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
				threadArgs[i].total = &total;

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