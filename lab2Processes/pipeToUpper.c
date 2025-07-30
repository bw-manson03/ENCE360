#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

int parent_to_child[2];
int child_to_parent[2];
static char message[BUFSIZ]; // BUFSIZ is the size of the built-in stdio buffer. 
int child_status, size;

#define INP 1
#define OUTP 0

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s message\n", argv[0]);
        exit(1);
    }

    /* Create pipes */
    if (pipe(parent_to_child) == -1) {
        perror("Pipe from");
        exit(2);
    }
    if (pipe(child_to_parent) == -1) {
        perror("Pipe - to");
        exit(2);
    }

    switch (fork()) { /* Fork a new process */

    case -1: /* Error forking */
        perror("Fork");
        exit(3);
        break; // Not needed, just good practice. 

    case 0: /* Child code */
        close(parent_to_child[INP]);
        close(child_to_parent[OUTP]);

        /* Read from parent */
        if (read(parent_to_child[OUTP], message, BUFSIZ) != -1) {
            printf("CHILD: Recieved %s\n", message);
        }
        else {
            perror("Read");
            exit(4);
        }

        for (int i = 0; message[i]; i++) {
            message[i] = toupper(message[i]);
        }

        write(child_to_parent[INP], message, strlen(message));

        /* Close pipes */
        close(parent_to_child[OUTP]);
        close(child_to_parent[INP]);
        break;

    default: /* Parent code */
        close(parent_to_child[OUTP]);
        close(child_to_parent[INP]);

        if (write(parent_to_child[INP], argv[1], strlen(argv[1])) != -1) {
            printf("PARENT: Sent %s\n", argv[1]);
        }
        else {
            perror("Write");
            exit(5);
        }

        if (read(child_to_parent[OUTP], message, BUFSIZ) != -1) {
            printf("PARENT: Recieved %s\n", message);
        }
        else {
            perror("Read");
            exit(5);
        }

        // Wait for the child to exit - don't worry too much about the details of this yet. 
        wait(&child_status);

        /* Close pipes */
        close(parent_to_child[INP]);
        close(child_to_parent[OUTP]);
    }

    return EXIT_SUCCESS;
}
