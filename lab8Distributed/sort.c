#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <time.h>
#include <assert.h>

// Utility timing function
float seconds(struct timespec start, struct timespec stop) {
    float diff = (stop.tv_sec - start.tv_sec) + (float)(stop.tv_nsec - start.tv_nsec) / (float)1e9;
    return diff;
}



//Allocate a square matrix
float *allocVec(int n) {
    float *res = (float*)malloc(sizeof(float) * n);
    memset(res, 0, sizeof(float) * n);

    return res;
}

// Fill a vector random values between 0 and 1
void randomVec(float *res, int n) {

    int i;
    for (i = 0; i < n; ++i) {
        res[i] = drand48();
    }
}



// Merge two arrays into one
void merge(float *a, int n1, float *b, int n2, float *arr) {
    int i = 0, j = 0, n = 0;

    // While there still exist elements in the source arrays
    while (i < n1 && j < n2) {
        // Add smallest element and increment
        if (a[i] < b[j]) {
            arr[n++] = a[i];
            i++;
        }
        else {
            arr[n++] = b[j];
            j++;
        }
    }

    // Merge uneven ends if there still exist values not merged
    while (i < n1) arr[n++] = a[i++];
    while (j < n2) arr[n++] = b[j++];
}

//Merge two arrays and allocate for the result
float *mergeAlloc(float *a, int n1, float *b, int n2) {
    float *res = (float*)malloc(sizeof(float) * (n1 + n2));
    merge(a, n1, b, n2, res);
    return res;
}


// Comparator function for qsort
int compare(const void *a, const void *b) {
    float *x = (float *)a;
    float *y = (float *)b;

    if (*x < *y) return -1;
    else if (*x > *y) return 1;
    else return 0;
}



// Round to the next power of 2 up
int nextPower2(int n) {
    int x = 1;

    while (x < n) {
        x = x << 1;
    }

    return x;
}


// Merge sorted arrays recursively
float *mergeTree(int *resultSize, float *subVec, int n1, int step) {

    // Get the rank of the process/number of processes
    int rank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    if (step > nextPower2(worldSize)) {
        // We are done
        *resultSize = n1;
        return subVec;
    }


    if (rank % step == 0) {

        // We are even with respect to the step size
        // wait for another sub vector from the "odd" side

        int source = rank + step / 2;
        if (source >= worldSize) {

            // No need to merge because we are at the edge (no pair)
            return mergeTree(resultSize, subVec, n1, step * 2);
        }
        else {
            //TODO: Implement me
            // Recieve the length of the vector, allocate space for it and recieve it
            int n2 = 0;
            float *otherVec = NULL;


            // Merge the two sub vectors together
            float *merged = mergeAlloc(subVec, n1, otherVec, n2);
            free(otherVec);

            // Recursively merge down
            return mergeTree(resultSize, merged, n1 + n2, step * 2);
        }

    }
    else {

        // We are odd with respect to the step size
        // send our vector to the even side and finish

        //TODO: Implement me
        // Send the length of the vector, followed by the data
        int dest = rank - step / 2;
        dest = dest; // Unneccessary: remove compiler warning



        //Free memory no longer needed
        free(subVec);

        *resultSize = 0;
        return NULL;
    }

}


// Test the result
int checkSorted(float *values, int n) {
    int i;
    for (i = 0; i < n - 1; ++i) {
        if (values[i] > values[i + 1]) {
            return 0;
        }
    }

    return 1;
}

int main(int argc, char** argv) {


    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the rank of the process/number of processes
    int worldRank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    if (argc < 2) {
        if (worldRank == 0) {
            printf("usage:  ./run.sh        sort numProcesses numElements\n");
        }

        MPI_Finalize();
        exit(1);
    }

    int totalElems = atoi(argv[1]);


    int elemsProc = (totalElems + (worldSize - 1)) / worldSize; // round up   

    float *randNums = NULL;

    int totalAlloc = elemsProc * worldSize;

    if (worldRank == 0) {
        srand48(0);

        // Allocate and initialize vector of random values
        randNums = allocVec(totalAlloc);
        randomVec(randNums, totalElems);
    }


    // How many extra elements exist
    int extra = totalAlloc - totalElems;

    // Trim the unused elements off the end of the last process
    int isLast = worldRank + 1 == worldSize;
    int localSize = elemsProc - (isLast ? extra : 0);
    if (worldRank >= totalElems) {
        localSize = 0;  // In the very odd case which we have more processes than elements
    }

    printf("initialized: rank %d of %d\n", worldRank, worldSize);

    //Start the clock
    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);

    // Create a buffer that will hold a subset of the random numbers
    float *subRandNums = allocVec(elemsProc);

    // Scatter the random numbers to all processes
    //TODO: Implement me

    // Use quicksort to sort the vectors at the local nodes
    qsort(subRandNums, localSize, sizeof(float), compare);

    // Finally use merge sort recursively to merge the vectors between processes
    int resultSize;
    float *result = mergeTree(&resultSize, subRandNums, localSize, 2);

    //Stop the clock
    clock_gettime(CLOCK_REALTIME, &stop);

    if (worldRank == 0) {
        assert(resultSize == totalElems);

        int correct = checkSorted(result, resultSize);
        printf("time taken: %.4f resultSize: %d, test: %s\n", seconds(start, stop), resultSize, correct ? "pass" : "fail");

        free(randNums);
        free(result);
    }

    //   Don't need to free subRandNums because mergeTree does so already

    // Cleanup
    MPI_Finalize();
    return 0;

}
