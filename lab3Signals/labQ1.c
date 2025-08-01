#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void handleSigquit(int sig);

void installHandler(void) {
    signal(SIGQUIT, handleSigquit);
}

void handleSigquit(int sig) {
    static int count = 0;
    (void)sig; // avoid unused parameter warning

    write(1, "SIGQUIT\n", 8);
    count++;

    if (count >= 2) {
        exit(0);
    }
}