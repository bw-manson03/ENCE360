#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define INP 1
#define OUTP 0

int main(void) {
    int pipe1[2];
    pid_t pid1;
    pipe(pipe1);
    
    if ((pid1 = fork()) == 0) {
        dup2(pipe1[INP], STDOUT_FILENO);
        
        close(pipe1[OUTP]);
        close(pipe1[INP]);
        
        execl("/usr/bin/sort", "sort", NULL);
        exit(1);
    }
    else {
        dup2(pipe1[OUTP], STDIN_FILENO);
        
        close(pipe1[OUTP]);
        close(pipe1[INP]);
        
        execl("/usr/bin/head", "head", "-2", NULL);
        exit(1);
    }
    exit(0);
}