

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


    int value = 0;

    int src = (worldRank + worldSize - 1) % worldSize;
    int dest = (worldRank + 1) % worldSize;

    //Send off the first message around the ring
    if (worldRank == 0) {
        printf("%d: sending value %d to process %d\n", worldRank, value, dest);
        MPI_Send(&value, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
    }

    //TODO: Implement me

    // Cleanup
    MPI_Finalize();
    return 0;
}