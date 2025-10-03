#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <time.h>

//Allocate a zero vector of length n
double *allocVec(int n) {
    double *res = (double*)malloc(sizeof(double) * n);
    memset(res, 0, sizeof(double) * n);

    return res;
}

// Fill a vector random values between 0 and 1
void randomVec(double *res, int n) {

    int i;
    for (i = 0; i < n; ++i) {
        res[i] = drand48();
    }
}

// Sum all elements in a vector
double computeSum(double *values, int count) {
    int i;
    double sum = 0.0;

    for (i = 0; i < count; ++i) {
        sum += values[i];
    }

    return sum;
}

// Square all elements in a vector
void square(double *values, int count) {
    int i;

    for (i = 0; i < count; ++i) {
        values[i] *= values[i];
    }
}

double seconds(struct timespec start, struct timespec stop) {
    double diff = (stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / (double)1e9;
    return diff;
}



int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the rank of the process
    int worldRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

    // Get the number of processes
    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    int totalElems = 100000;
    int elemsProc = (totalElems + (worldSize - 1)) / worldSize; // round up


    double *randNums = NULL;

    if (worldRank == 0) {
        srand48(0);

        randNums = allocVec(elemsProc * worldSize);
        randomVec(randNums, totalElems);
    }

    printf("initialized: rank %d of %d\n", worldRank, worldSize);

    // Start the clock
    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);


    // Create a buffer that will hold a subset of the random numbers
    double *subRandNums = allocVec(elemsProc);

    // Scatter the random numbers to all processes
    MPI_Scatter(randNums, elemsProc, MPI_DOUBLE, subRandNums, elemsProc, MPI_DOUBLE, 0, MPI_COMM_WORLD);


    // square each element
    square(subRandNums, elemsProc);

    // Compute the sum of the subset on all processes 
    double localLen = computeSum(subRandNums, elemsProc);

    double total = 0.0;

    //TODO: Replace MPI_Reduce with MPI_Gather impelemtation
    MPI_Reduce(&localLen, &total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);







    
    
    
    
    
    
    
    // Stop the clock
    clock_gettime(CLOCK_REALTIME, &stop);

    // Compute the total average of all numbers.
    if (worldRank == 0) {
        printf("time taken: %.4f vector length: %f\n", seconds(start, stop), sqrt(total));
    }


    // Cleanup
    MPI_Finalize();
    return 0;

}