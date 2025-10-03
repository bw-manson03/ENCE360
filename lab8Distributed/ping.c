#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>



int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    // Get the rank of the process
    int worldRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

    if (worldSize % 2) {
        fprintf(stderr, "error: please use an even number of processes\n");
        fprintf(stderr, "usage: ./run.sh ./ping <number of processes>\n");

        MPI_Finalize();
        exit(1);
    }

    int value = 0;
    while (value < 10) {

        if (worldRank % 2 == 1) {
            // Wait for a value back from the client
            MPI_Recv(&value, 1, MPI_INT, worldRank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%d: sending pong %d to %d \n", worldRank, value, worldRank - 1);

            value++;
            // Send the value to the client
            MPI_Send(&value, 1, MPI_INT, worldRank - 1, 0, MPI_COMM_WORLD);
        }
        else {
            // Send the value to the client
            MPI_Send(&value, 1, MPI_INT, worldRank + 1, 0, MPI_COMM_WORLD);
            printf("%d: sending ping %d to %d \n", worldRank, value, worldRank + 1);

            // Wait for a value back from the client
            MPI_Recv(&value, 1, MPI_INT, worldRank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }


    // Cleanup
    MPI_Finalize();
    return 0;
}